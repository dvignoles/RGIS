FROM ubuntu:19.04
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y gnupg gnupg-utils lsb-base lsb-release wget ca-certificates && \
    wget --quiet -O - https://www.postgresql.org/media/keys/ACCC4CF8.asc | apt-key add - && \
    echo "deb http://apt.postgresql.org/pub/repos/apt/ $(lsb_release -cs)-pgdg main" >> /etc/apt/sources.list.d/pgdg.list && \
    apt-get update && apt-get install -y --no-install-recommends dialog apt-utils apt-transport-https dnsutils psmisc \
                       git vim cmake clang rsync screen tmux bc curl sudo nfs-common python python-numpy python-psutil \
                       libnetcdf-dev netcdf-bin nco cdo libudunits2-0 libudunits2-data libudunits2-dev \
                       libexpat1 libexpat1-dev libxext-dev gnuplot gdal-bin libgdal-dev imagemagick synaptic \
                       python-gdal python3-gdal postgresql-client-11 postgis libexpat1 libexpat1-dev \
                       libmotif-common libmotif-dev libxext-dev libxss1 && rm -rf /var/lib/apt/lists/*

# Install GHAAS
RUN git clone https://github.com/bmfekete/RGIS /tmp/RGIS && /tmp/RGIS/install.sh /usr/local/share && rm -rf /tmp/RGIS
ENV PATH="${PATH}:/usr/local/share/ghaas/bin:/usr/local/share/ghaas/f"

ENTRYPOINT ["/bin/bash"]
