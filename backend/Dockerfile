# -------- BUILD STAGE --------
# Use Drogon base image (has compiler, CMake, Drogon libs preinstalled)
FROM drogonframework/drogon:latest AS builder

# Work inside /src
WORKDIR /src

# Copy backend folder (contains CMakeLists.txt + src + header)
COPY ./backend .

# Run CMake to configure and build your C++ app
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
 && cmake --build build -j


# -------- RUNTIME STAGE --------
# Start a new lightweight container with Drogon runtime
FROM drogonframework/drogon:latest

# Work inside /backend
WORKDIR /backend

# Copy the compiled binary from the builder into this runtime container
COPY --from=builder /src/build/StudentManagementSystem /backend/server

# Copy static frontend files from the "docs" folder into the container
# They will be served from /app/docs
COPY ./docs /docs

# Set environment variable for port
ENV PORT=8080

# Tell Docker this app listens on port 8080
EXPOSE 8080

# Run the Drogon binary when the container starts
CMD ["/backend/server"]
