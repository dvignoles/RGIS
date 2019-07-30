FROM linuxmintd/mint19-amd64
RUN apt-get update && DEBIAN_FRONTEND=noninteractive \
    apt-get install -y --no-install-recommends dialog apt-utils apt-transport-https ca-certificates dnsutils psmisc \
                       git vim cmake clang rsync screen tmux bc curl nfs-common python python-numpy python-psutil \
                       libnetcdf-dev netcdf-bin nco cdo libudunits2-0 libudunits2-data libudunits2-dev \
                       libexpat1 libexpat1-dev libxext-dev gnuplot gdal-bin libgdal-dev synaptic \
                       python-gdal python3-gdal postgresql-client postgis libexpat1 libexpat1-dev \
                       libmotif-common libmotif-dev libxext-dev libxss1 && rm -rf /var/lib/apt/lists/*

# Install GHAAS
RUN git clone https://github.com/bmfekete/RGIS /tmp/RGIS && /tmp/RGIS/install.sh /usr/local/share && rm -rf /tmp/RGIS
ENV PATH="${PATH}:/usr/local/share/ghaas/bin:/usr/local/share/ghaas/f"

ENTRYPOINT ["/bin/bash"]
