FROM debian:bookworm-slim

# Install build dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        libreadline-dev \
        libncurses-dev && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy all source files
COPY . .

# Build the project
RUN make fclean && make

ENTRYPOINT ["./bin/minishell"]
