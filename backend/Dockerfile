# -------- BUILD STAGE --------
FROM drogonframework/drogon:latest AS builder
WORKDIR /src

# Use MariaDB headers (avoid conflict with libmysqlclient-dev)
RUN apt-get update && apt-get install -y --no-install-recommends \
    libmariadb-dev libmariadb-dev-compat \
    && rm -rf /var/lib/apt/lists/*

# Copy backend sources (CMakeLists.txt + src/ + config/)
COPY ./backend .

# Build
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build -j

# -------- RUNTIME STAGE --------
FROM drogonframework/drogon:latest
WORKDIR /backend

# Runtime client library
RUN apt-get update && apt-get install -y --no-install-recommends \
    libmariadb3 ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Copy binary & config
COPY --from=builder /src/build/StudentManagementSystem /backend/server
COPY ./backend/config/mysqlconfig.json /backend/config/mysqlconfig.json

ENV PORT=8080
EXPOSE 8080
CMD ["/backend/server", "-c", "/backend/config/mysqlconfig.json"]
