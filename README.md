<img src="https://github.com/kichkiro/42/blob/assets/banner_ft_ping.png?raw=true" width="100%"/>

<i>
  <p>
    This project is about recoding the ping command.
  </p>
</i>

#### <i>[subject](_subject/en.subject.pdf) v.5.1</i>

<details>
<summary><i><b>Project Structure  📂</b></i></summary>

``` js
.
├── Dockerfile
├── LICENSE
├── README.md
├── _subject
│   └── en.subject.pdf
└── project
    ├── Makefile
    ├── ft_ping
    ├── include
    │   └── ft_ping.h
    └── src
        ├── libft
        │   ├── ft_free.c
        │   ├── ft_int_len.c
        │   ├── ft_str_realloc.c
        │   └── ft_strf.c
        ├── logger.c
        ├── main.c
        ├── packets.c
        ├── parser.c
        ├── ping.c
        ├── socket.c
        └── verbose.c
```
</details>

## 📑 Index
- [Usage](#️-usage)
  - [Debian-based OS](#-debian-based-os)
  - [Docker](#-docker)
- [Key Topics](#-key-topics)
  - [PING (GNU inetutils v2.0)](#-ping-gnu-inetutils-v20)
  - [RAW Sockets](#-raw-sockets)
  - [IPv4 Header](#-ipv4-header)
  - [ICMP](#-icmp)
    - [ICMP Header](#--icmp-header)
    - [ICMP Type](#--icmp-type)
    - [ICMP Code](#--icmp-code)
    - [ICMP Payload](#--icmp-payload)
    - [ICMP Checksum](#--icmp-checksum)
  - [Packet Inspection](#-packet-inspection)
    - [GDB](#--gdb)
    - [Wireshark](#--wireshark)
- [References](#-references)
- [License](#-license)

## 🛠️ Usage
After cloning the repository, you can opt to install it on a Debian-based 
system, or via Docker for greater portability.

### [+] Debian-based OS
In this case you will need to make sure you have all the dependencies, or 
install the meta-packege `build-essential`, which already contains all the tools 
needed to compile the program:

```sh
apt-get install -y --no-install-recommends build-essential
```

and then:

```sh
cd ft_ping/project
make
./ft_ping [COMMAND] [OPTIONS]
```


### [+] Docker
If you choose installation via Docker, you don't need anything other than Docker 
itself.

```sh
cd ft_ping/
docker build -t ft_ping .
docker run --rm ft_ping [COMMAND] [OPTIONS]
```

## 📌 Key Topics

### [+] PING (GNU inetutils v2.0)
This reimplementation of `ping` is inspired by the version of 
`GNU inetutils v2.0`, below is a brief introduction and installation guide for 
testing purposes.

The `ping` command sends __ICMP ECHO_REQUEST__ packets to a host to check 
connectivity. 

It is commonly used to:

- Verify network status.
- Diagnose connectivity issues.
- Measure response times and packet loss.

Each request includes IP and ICMP headers plus timing data. 
By default, `ping` sends packets once per second until interrupted (Ctrl-C). 
It reports round-trip times, statistics, and a summary upon completion. 
The target can be a hostname or IP address.

__Installing GNU inetutils 2.0 on Debian-based systems:__

1. Remove the default ping:
   ```bash
   apt remove iputils-ping
   ```

2. Install build tools:
   ```bash
   apt update
   apt install build-essential wget
   ```

3. Download version 2.0 sources:
   ```bash
   wget https://ftp.gnu.org/gnu/inetutils/inetutils-2.0.tar.gz
   ```

4. Extract and compile:
   ```bash
   tar -xvf inetutils-2.0.tar.gz
   cd inetutils-2.0
   ./configure --prefix=/usr/local/inetutils-2.0
   make
   make install
   ```

5. Use the specific binary:
   ```bash
   /usr/local/inetutils-2.0/bin/ping --version
   ```

   or create an alias:
   ```bash
   echo "alias ping='/usr/local/inetutils-2.0/bin/ping'" >> ~/.bashrc
   source ~/.bashrc
   ```

### [+] RAW Sockets

This implementation uses __raw sockets__ to craft and send fully custom ICMP packets, including the IPv4 header.
Two separate sockets are created:

- A raw IP socket `socket(AF_INET, SOCK_RAW, IPPROTO_RAW)` for sending packets with manually constructed IPv4 headers.

- A raw ICMP socket `socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)` for receiving ICMP Echo Replies and other ICMP messages.

With raw sockets and the `IP_HDRINCL` socket option enabled, the application takes full control over the entire packet structure:

- The kernel does not generate the IPv4 header automatically. Instead, your program must manually construct the IPv4 header, specifying all fields (Version, Header Length, Total Length, Identification, TTL, Protocol, etc.).

- You must also build the ICMP header yourself, setting the Type, Code, Identifier, Sequence Number, and computing the correct checksum.

- A custom payload is appended to the ICMP packet, it includes a timestamp and arbitrary data for testing round-trip times and verifying packet integrity.

- On reception, the program receives the full raw IP packet (including the IPv4 header), so it must parse the header manually to extract and validate the embedded ICMP Echo Reply.

__Note:__

- Without `IP_HDRINCL`, the kernel would automatically fill in the IPv4 header based on your destination address and other defaults, and you would only need to provide the transport-layer payload (ICMP). With `IP_HDRINCL`, you get full control but also full responsibility.

- Because raw sockets allow generating arbitrary network traffic, they require elevated privileges (root or `CAP_NET_RAW` capability).

### [+] IPv4 Header

Each packet sent by this implementation contains a manually constructed IPv4 header.  
Below is an overview of its fields:

| Field               | Size (bits) | Description                                                    |
|---------------------|-------------|----------------------------------------------------------------|
| __Version__         | 4           | IP protocol version. Always set to 4 for IPv4.                |
| __IHL__             | 4           | Internet Header Length in 32-bit words. Usually 5 (20 bytes). |
| __Type of Service__ | 8           | Specifies priority and QoS.                                   |
| __Total Length__    | 16          | Full length of the packet (header + payload).                 |
| __Identification__  | 16          | Unique identifier for fragmentation.                          |
| __Flags__           | 3           | Control flags (e.g., Don't Fragment).                         |
| __Fragment Offset__ | 13          | Offset of a fragment in the original datagram.                |
| __TTL__             | 8           | Time To Live, decremented by each router hop.                 |
| __Protocol__        | 8           | Encapsulated protocol number (1 for ICMP).                    |
| __Header Checksum__ | 16          | Checksum of the header. Calculated manually.                  |
| __Source Address__  | 32          | IPv4 address of the sender.                                   |
| __Destination Address__ | 32      | IPv4 address of the recipient.                                |

__Note:__
- The `IP_HDRINCL` socket option requires the application to populate all fields and compute the header checksum.
- The "Don't Fragment" flag is set to avoid fragmentation.
- The source address can be set to `INADDR_ANY`, in which case the kernel fill in the actual address.

### [+] ICMP

The __Internet Control Message Protocol (ICMP)__ is part of the Internet protocol suite ([RFC 792](https://www.rfc-editor.org/rfc/rfc792.html)). It is used by network devices to send error messages and operational information, such as when a host is unreachable or a service is unavailable. Unlike TCP and UDP, ICMP is mainly for diagnostics (e.g., *ping*, *traceroute*) and error reporting ([RFC 1122](https://www.rfc-editor.org/rfc/rfc1122.html)).

ICMP operates at __Layer 3 (the Network Layer)__ of the OSI model, the same layer as IP.

#### - ICMP Header

Each ICMP message starts with a fixed header:

| Field              | Size (bits) | Description                                                |
|--------------------|-------------|------------------------------------------------------------|
| __Type__           | 8           | Specifies the ICMP message type (e.g., 0=Echo Reply, 8=Echo Request). |
| __Code__           | 8           | Further refines the message type.                          |
| __Checksum__       | 16          | Verifies the integrity of the ICMP header and payload.     |
| __Rest of Header__ | 32          | Depends on Type and Code. For Echo messages, contains Identifier and Sequence Number. |

__Echo Request/Reply additional fields:__

| Subfield       | Size (bits) | Description                      |
|----------------|-------------|----------------------------------|
| __Identifier__ | 16          | Helps match requests to replies. |
| __Sequence__   | 16          | Incremented with each request.   |

#### - ICMP Type

These message types define ICMP control and diagnostic functions as specified in 
[RFC 792](https://www.rfc-editor.org/rfc/rfc792.html) and related updates.  
Each Type identifies the high-level purpose of the message, while the Code field 
provides additional context.
Below are common ICMP types:

| Type | Name                         | Description                                       |
|------|------------------------------|---------------------------------------------------|
| 0    | Echo Reply                   | Reply to an Echo Request.                        |
| 3    | Destination Unreachable      | Packet could not be delivered.                   |
| 5    | Redirect                     | Redirect routing (e.g., better gateway).         |
| 8    | Echo Request                 | Ping request.                                    |
| 9    | Router Advertisement         | Router discovery messages.                       |
| 10   | Router Solicitation          | Request for router advertisement.                |
| 11   | Time Exceeded                | TTL expired in transit.                          |
| 12   | Parameter Problem            | Invalid header field.                            |
| 13   | Timestamp Request            | Request timestamp.                               |
| 14   | Timestamp Reply              | Reply to timestamp request.                      |

#### - ICMP Code

The meaning of the __Code__ field depends on the Type.  
Below are common codes for a few types:

| Type | Code | Meaning                                  |
|------|------|------------------------------------------|
| 3    | 0    | Network Unreachable                      |
| 3    | 1    | Host Unreachable                         |
| 3    | 2    | Protocol Unreachable                     |
| 3    | 3    | Port Unreachable                         |
| 3    | 4    | Fragmentation needed and DF set         |
| 3    | 5    | Source Route Failed                     |
| 11   | 0    | TTL expired in transit                  |
| 11   | 1    | Fragment reassembly time exceeded       |

#### - ICMP Payload

The ICMP payload immediately follows the header.  
For Echo Request and Echo Reply packets, it typically contains:

- A __timestamp__ (e.g., `struct timeval`) used to measure round-trip time (RTT).
- Arbitrary __data bytes__ (padding) to reach the desired packet size.

#### - ICMP Checksum

The __checksum__ field covers the entire ICMP message, including both the header 
and the payload.
It is calculated as the one's complement of the one's complement sum of all 
16-bit words in the message.

If the total length of the message is odd, the final byte is padded with a zero 
byte before computing the checksum.

__Formula:__
checksum = ~ (sum of all 16-bit words)

__Note:__  
A valid checksum is required for the receiver to accept and process the ICMP packet.
If the checksum is incorrect, the packet will be discarded.

### [+] Packet Inspection

You can inspect the data both __live during execution__, using GDB and __passively__ using Wireshark.

#### - GDB

When you receive a packet via `recvfrom()`, the buffer contains the __entire raw IP packet__, starting from the first byte of the IPv4 header (offset 0).

This means:
- Offset 0–19: IPv4 header (fixed part, 20 bytes)
- Offset 20–N: ICMP header and payload

Each field occupies specific offsets in the buffer, matching the protocol specification (RFC 791 for IPv4, RFC 792 for ICMP).

In the example below, we display the full packet ([IPv4 header] + [ICMP header] + [ICMP payload]), which by default is 84 bytes long:

```yaml
(gdb) x/84bx rbuf 
0x55555555db00: 0x45    0x00    0x00    0x54    0x34    0x48    0x00    0x00
0x55555555db08: 0xfc    0x01    0x78    0x30    0x0a    0x00    0x00    0x01
0x55555555db10: 0xac    0x1e    0x5c    0x11    0x00    0x00    0x90    0x88
0x55555555db18: 0xe7    0xc3    0x00    0x00    0xee    0xea    0x66    0x68
0x55555555db20: 0x00    0x00    0x00    0x00    0xae    0xce    0x06    0x00
0x55555555db28: 0x00    0x00    0x00    0x00    0x00    0x01    0x02    0x03
0x55555555db30: 0x04    0x05    0x06    0x07    0x08    0x09    0x0a    0x0b
0x55555555db38: 0x0c    0x0d    0x0e    0x0f    0x10    0x11    0x12    0x13
0x55555555db40: 0x14    0x15    0x16    0x17    0x18    0x19    0x1a    0x1b
0x55555555db48: 0x1c    0x1d    0x1e    0x1f    0x20    0x21    0x22    0x23
0x55555555db50: 0x24    0x25    0x26    0x27
```

Knowing the offset of each field inside the IPv4 header, you can extract and print readable values.

For example the source IP address is located between byte 12 and byte 15 of the IPv4 header and destination between byte 16 and byte 19, you can print it as a human-readable string like this:
```c
(gdb) printf "%d.%d.%d.%d\n", rbuf[12], rbuf[13], rbuf[14], rbuf[15]
10.0.0.1
(gdb) printf "%d.%d.%d.%d\n", rbuf[16], rbuf[17], rbuf[18], rbuf[19]
172.30.92.17
```

#### - Wireshark
Wireshark can capture and decode ICMP packets in real time.

It provides a detailed, human-readable view of each field in the IPv4 and ICMP headers, making it an essential tool to verify that raw packets are crafted correctly.

You can filter the traffic with some useful filters:
- `icmp` – Show all ICMP packets (requests and replies).
- `icmp.type == 8` – Show only Echo Requests.
- `icmp.type == 0` – Show only Echo Replies.
- `ip.src == <your_ip>` – Filter packets sent by your host.
- `ip.dst == <target_ip>` – Filter packets sent to the target.

<img src="https://github.com/kichkiro/42/blob/assets/ft_ping_wireshark.png?raw=true" width="100%"/>

Below is an example of an ICMP Echo Request packet as dissected by Wireshark:

``` yaml
Internet Protocol Version 4, Src: 172.30.92.17, Dst: 10.0.0.1
    0100 .... = Version: 4
    .... 0101 = Header Length: 20 bytes (5)
    Differentiated Services Field: 0x00 (DSCP: CS0, ECN: Not-ECT)
    Total Length: 84
    Identification: 0xcecd (52941)
    010. .... = Flags: 0x2, Don't fragment
    ...0 0000 0000 0000 = Fragment Offset: 0
    Time to Live: 64
    Protocol: ICMP (1)
    Header Checksum: 0x59ab [validation disabled]
    [Header checksum status: Unverified]
    Source Address: 172.30.92.17
    Destination Address: 10.0.0.1
    [Stream index: 1]
Internet Control Message Protocol
    Type: 8 (Echo (ping) request)
    Code: 0
    Checksum: 0xb471 [correct]
    [Checksum Status: Good]
    Identifier (BE): 52941 (0xcecd)
    Identifier (LE): 52686 (0xcdce)
    Sequence Number (BE): 0 (0x0000)
    Sequence Number (LE): 0 (0x0000)
    [Response frame: 8]
    Timestamp from icmp data: Jul  3, 2025 22:23:51.516017000 W. Europe Daylight Time
    [Timestamp from icmp data (relative): -0.303915000 seconds]
    Data (40 bytes)
        Data: 000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2021222324252627
        [Length: 40]

```

Here is the same packet shown as a raw hex dump:
```yaml
# IPv4 Header (20 bytes)
45 00 00 54 ce cd 40 00 40 01 59 ab ac 1e 5c 11
0a 00 00 01
# ICMP Header + Payload
08 00 b4 71 ce cd 00 00 d7 e6 66 68 00 00 00 00
b1 df 07 00 00 00 00 00 00 01 02 03 04 05 06 07
08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17
18 19 1a 1b 1c 1d 1e 1f 20 21 22 23 24 25 26 27
```

## 📚 References

- RFC
  - [RFC 791 - Internet Protocol](https://www.rfc-editor.org/rfc/rfc791.html)
  - [RFC 792 - ICMP](https://www.rfc-editor.org/rfc/rfc792.html)
  - [RFC 1071 - Computing the Internet Checksum](https://www.rfc-editor.org/rfc/rfc1071.html)
  - [RFC 1122 - Requirements for Internet Hosts](https://www.rfc-editor.org/rfc/rfc1122.html)

- Man Pages
  - [man 7 raw](https://man7.org/linux/man-pages/man7/raw.7.html)
  - [man 2 socket](https://man7.org/linux/man-pages/man2/socket.2.html) 
  - [man 2 setsockopt](https://man7.org/linux/man-pages/man2/setsockopt.2.html) 
  - [man 2 sendto](https://man7.org/linux/man-pages/man2/sendto.2.html) 
  - [man 2 recvfrom](https://man7.org/linux/man-pages/man2/recvfrom.2.html) 
  - [man 7 ip](https://man7.org/linux/man-pages/man7/ip.7.html) 

- Other
  - [Wireshark](https://www.wireshark.org/)
  - [IANA ICMP Parameters](https://www.iana.org/assignments/icmp-parameters/icmp-parameters.xhtml)
  - [Ping Wikipedia](https://en.wikipedia.org/wiki/Ping_(networking_utility))

## ⚖️ License
See [LICENSE](./LICENSE)
