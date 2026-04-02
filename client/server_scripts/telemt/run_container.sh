# Run container
sudo docker run -d \
  --log-driver none \
  --restart always \
  -p $TELEMT_PORT:$TELEMT_PORT/tcp \
  -v amnezia-telemt-data:/data \
  --name $CONTAINER_NAME \
  $CONTAINER_NAME
