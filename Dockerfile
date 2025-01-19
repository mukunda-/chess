# A dockerfile to build and run tests (and you can extract binaries from here too).
# $ docker build .
FROM silkeh/clang

WORKDIR /app

# Install required bison, flex
# valgrind for testing.
RUN apt-get update \
    && apt-get install -y bison flex valgrind

COPY . .

# Build everything and test it.
RUN make all
RUN make test

# Additional commands as necessary...
