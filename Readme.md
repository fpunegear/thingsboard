# Thingboard con docker y postgres

### Url de guia
https://thingsboard.io/docs/user-guide/install/docker-windows/

## Pre requisitos
- docker

## Ejecutar docker compose

```
# Para iniciar el contenedor
docker compose up -d

# Para visualizar los logs
docker compose logs -f mytb
```

## Donde corre

```
http://localhost:8080
```

## Usuarios por defecto
- System Administrator: sysadmin@thingsboard.org / sysadmin
- Tenant Administrator: tenant@thingsboard.org / tenant
- Customer User: customer@thingsboard.org / customer
