docker build -t rgis-docker RGIS
docker tag  rgis-docker gcr.io/compass-236716/rgis-docker:latest
docker push gcr.io/compass-236716/rgis-docker:latest
