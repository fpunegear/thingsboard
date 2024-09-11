# Thingboard con docker y postgres
### Url de guia
https://thingsboard.io/docs/user-guide/install/docker-windows/

## Pre requisitos
- docker

## Volumes
Usuarios windows deben administrar volumenes

docker volume create mytb-data
docker volume create mytb-logs


## Ejecutar docker compose

docker compose up -d
docker compose logs -f mytb


## Donde corre
http://localhost:8080

## Usuarios por defecto
- System Administrator: sysadmin@thingsboard.org / sysadmin
- Tenant Administrator: tenant@thingsboard.org / tenant
- Customer User: customer@thingsboard.org / customer
