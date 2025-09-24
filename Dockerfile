# Use a base image with C++ build tools
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    && rm -rf /var/lib/apt/lists/*

# Set working directoryd
WORKDIR /chess-engine

# Copy the entire project
COPY . .

# Create build directory
RUN mkdir -p build

# Build argument to specify mode (tests/cli/python-lib)
ARG BUILD_MODE=tests
ENV BUILD_MODE=${BUILD_MODE}

# Script to handle different build modes
RUN echo '#!/bin/bash\n\
if [ "$BUILD_MODE" = "tests" ]; then\n\
    cd build && \\\n\
    cmake -DMode=tests .. && \\\n\
    make -j && \\\n\
    cd chess_board_tests && \\\n\
    ctest --output-on-failure \n\
elif [ "$BUILD_MODE" = "cli" ]; then\n\
    echo "CLI mode not implemented yet"\n\
    exit 1\n\
elif [ "$BUILD_MODE" = "python-lib" ]; then\n\
    echo "Python library mode not implemented yet"\n\
    exit 1\n\
else\n\
    echo "Invalid build mode. Use: tests, cli, or python-lib"\n\
    exit 1\n\
fi' > /entrypoint.sh && chmod +x /entrypoint.sh

# Set the entry point
ENTRYPOINT ["/entrypoint.sh"]