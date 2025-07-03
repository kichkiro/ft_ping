FROM debian:bookworm-slim AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends build-essential && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /build/ft_ping

COPY project/ .

RUN make

FROM debian:bookworm-slim

COPY --from=build /build/ft_ping/ft_ping /usr/local/bin/ft_ping

ENTRYPOINT ["ft_ping"]

