FROM    ubuntu:19.04
RUN apt-get update && DEBIAN_FRONTEND=noninteractive \
    apt-get install -y apt-utils git vim cmake python python-numpy \
                       libnetcdf-dev netcdf-bin nco cdo \
                       libudunits2-0 libudunits2-data libudunits2-dev \
                       libexpat1 libexpat1-dev libxext-dev \
                       libmotif-common libmotif-dev gdal-bin gdal-data libgdal-dev \
                       postgis postgis-doc postgresql-client && rm -rf /var/lib/apt/lists/*

# Install GHAAS
RUN git clone https://github.com/bmfekete/RGIS /tmp/RGIS && /tmp/RGIS/install.sh /usr/local/share && rm -rf /tmp/RGIS
ENV PATH="${PATH}:/usr/local/share/ghaas/bin:/usr/local/share/ghaas/f"

ENTRYPOINT ["/bin/bash"]
