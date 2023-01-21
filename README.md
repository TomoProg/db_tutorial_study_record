# DB tutorial study record

## Environment
```
# docker -v
Docker version 20.10.3, build 48d30b5

# docker-compose --version
Docker version 20.10.3, build 48d30b5
```

## How to setup
```
docker-compose up --build
docker exec db_tutorial_gcc make
```

## How to test
```
docker exec db_tutorial_rspec bundle install
docker exec db_tutorial_rspec make test
```

