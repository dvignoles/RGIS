FROM    ubuntu:18.04
RUN apt-get update && DEBIAN_FRONTEND=noninteractive \
    apt-get install -y apt-utils vim git cmake libnetcdf-dev netcdf-bin \
                       libudunits2-0 libudunits2-data libudunits2-dev   \
                       libexpat1 libexpat1-dev libxext-dev \
                       libmotif-common libmotif-dev gdal-bin gdal-data libgdal-dev \
                       postgis postgis-doc postgresql10-postgis-2.4 && rm -rf /var/lib/apt/lists/*

# Always use UTC on a server
RUN ln -snf /usr/share/zoneinfo/UTC /etc/localtime && echo UTC > /etc/timezone

# Install GHAAS
RUN git clone https://github.com/bmfekete/RGIS /tmp/RGIS && /tmp/RGIS/install.sh /usr/local/share && rm -rf /tmp/RGIS
ENV PATH="${PATH}:/usr/local/share/ghaas/bin:/usr/local/share/ghaas/f"
