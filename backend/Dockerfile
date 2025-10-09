# -------- BUILD STAGE --------
FROM drogonframework/drogon:latest AS builder
WORKDIR /src

# Use MariaDB headers (avoid conflict with libmysqlclient-dev)
RUN apt-get update && apt-get install -y --no-install-recommends \
    libsodium-dev libmariadb-dev libmariadb-dev-compat \
    && rm -rf /var/lib/apt/lists/*

# Copy backend sources (CMakeLists.txt + src/ + config/)
COPY ./backend .

# Build
# Kept --parallel $(nproc) for efficient builds across OSes
RUN rm -rf build \
    && cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --parallel $(nproc)

# -------- RUNTIME STAGE --------
FROM drogonframework/drogon:latest
WORKDIR /backend

# Runtime client library
RUN apt-get update && apt-get install -y --no-install-recommends \
    libmariadb3 ca-certificates \
    libsodium23 \
    && rm -rf /var/lib/apt/lists/*

# Copy binary & config
COPY --from=builder /src/build/StudentManagementSystem /backend/server

# Copy config + static files
COPY ./backend/config /backend/config
COPY ./docs /docs


ENV PORT=8080
EXPOSE ${PORT}

CMD ["/backend/server"]
