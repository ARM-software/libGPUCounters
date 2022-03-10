FROM mirrors--dockerhub.eu-west-1.artifactory.aws.arm.com/ubuntu:20.04

# Jenkins image build job uses this label to tag version of image
# Update when you make a change
LABEL build.environment.version="1.0.0"

RUN useradd -u 1000 -U -m -c Jenkins jenkins

RUN apt update && apt -y upgrade \
  && apt install -y \
    ca-certificates \
    clang \
    clang++-9 \
    g++ \
    gcc \
    git \
    gnupg \
    imagemagick \
    make \
    python3 \
    python3-numpy \
    python3-pil \
    python3-pip \
    python3-venv \
    software-properties-common \
    wget

# Install up-to-date CMake, as standard Ubuntu 18.04 package is too old
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null \
    | gpg --dearmor - > /etc/apt/trusted.gpg.d/kitware.gpg
RUN apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'
RUN apt-get update
RUN apt-get install -y cmake \
  && rm -rf /var/lib/apt/lists/*


# Install python modules
RUN pip3 install requests

# Install Coverity client
RUN --mount=type=secret,id=ARTIFACTORY_CREDENTIALS curl --user "$(cat /run/secrets/ARTIFACTORY_CREDENTIALS)" "https://eu-west-1.artifactory.aws.arm.com/artifactory/mobile-studio.tools/coverity/cov-analysis-linux64-2020.06.sh" --output /tmp/coverity_install.sh &&\
  curl --user "$(cat /run/secrets/ARTIFACTORY_CREDENTIALS)" "https://eu-west-1.artifactory.aws.arm.com/artifactory/mobile-studio.tools/coverity/license.dat" --output /tmp/license.dat &&\
  chmod 555 /tmp/coverity_install.sh &&\
  /tmp/coverity_install.sh -q --license.region=6 --license.agreement=agree --license.cov.path=/tmp/license.dat -dir /usr/local/cov-analysis &&\
  rm /tmp/coverity_install.sh /tmp/license.dat
