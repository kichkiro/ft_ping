# ft_ping

<!-- ![ft_ping](https://github.com/kichkiro/42_cursus/blob/assets/banner_ft_ping.jpg?raw=true) -->

<i>
  <p>
    This project is about recoding the ping command.
  </p>
</i>

#### <i>[subject](_subject/en.subject.pdf) v.5.1</i>

## 🛠️ - Usage

## 📌 - Key Topics

### ICMP 

The Internet Control Message Protocol (ICMP) is a supporting protocol in the Internet protocol suite. It is used by network devices, including routers, to send error messages and operational information indicating success or failure when communicating with another IP address. For example, an error is indicated when a requested service is not available or that a host or router could not be reached. ICMP differs from transport protocols such as TCP and UDP in that it is not typically used to exchange data between systems, nor is it regularly employed by end-user network applications (with the exception of some diagnostic tools like ping and traceroute).

ICMP is part of the Internet protocol suite as defined in [RFC 792](https://www.rfc-editor.org/rfc/rfc792.html). ICMP messages are typically used for diagnostic or control purposes or generated in response to errors in IP operations (as specified in [RFC 1122](https://www.rfc-editor.org/rfc/rfc1122.html)). ICMP errors are directed to the source IP address of the originating packet.

### PING (GNU inetutils v2.0)

The /usr/sbin/ping command sends an Internet Control Message Protocol (ICMP) ECHO_REQUEST to obtain an ICMP ECHO_RESPONSE from a host or gateway. The ping command is useful for:

- Determining the status of the network and various foreign hosts.
- Tracking and isolating hardware and software problems.
- Testing, measuring, and managing networks.

If the host is operational and on the network, it responds to the echo. 

Each echo request contains an __Internet Protocol (IP)__ and __ICMP header__, followed by a __ping PID__ and a __timeval structure__, and __enough bytes to fill out the packet__. 

The default is to continuously send echo requests until an Interrupt is received (Ctrl-C).

The ping command sends one datagram per second and prints one line of output for every response received. 

The ping command calculates round-trip times and packet loss statistics, and displays a brief summary on completion. 

The ping command completes when the program times out or on receipt of a __SIGINT__ signal. 

The Host parameter is either a valid host name or Internet address.

By default, the ping command will continue to send echo requests to the display until an Interrupt is received (Ctrl-C). 

The Interrupt key can be changed by using the stty command.

Because of the load that continuous echo requests can place on the system, repeated requests should be used primarily for problem isolation.


## 📚 - References
- RFC
  - [RFC 792 - ICMP](https://www.rfc-editor.org/rfc/rfc792.html)
  - [RFC 1122 - Requirements for Internet Hosts](https://www.rfc-editor.org/rfc/rfc1122.html)


## ⚖️ - License
See [LICENSE](./LICENSE)
