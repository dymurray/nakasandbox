# Nakasendo Sandbox
** Powered by nChain's Nakasendo libraries **

This repository provides a sample Dockerfile which can be used to build a CentOS based image with the Nakasendo SDK installed along with its dependencies. I have published a built image on [DockerHub](https://hub.docker.com/r/dymurray/nakasendo/) which you can pull and immediately use.

## Prerequisites
* docker

## Usage
Currently, all building and execution should be done from within the container. The linking is done dynamically so unless your host has the libraries preinstalled then you cannot execute the binary outside of the container. Regardless, I recommend you mount your source directly into the container like so:
```
docker run -it -v $PWD:/home/nakasendo/<app_name>:Z docker.io/dymurray/nakasendo
```

Note: the `:Z` argument to the volume mount is required since selinux is enabled within the container.

## Example
I included in this repo an example test application (ripped from nChain example) which demonstrates Key Derivation from a private/pub key pair.
```
$ docker run -it -v $PWD/examples/c++/testapp:/home/nakasendo/testapp:Z docker.io/dymurray/nakasendo
```
Now from within the container we can build and run the compiled binary:
```
sh-4.2$ cd testapp/
sh-4.2$ make
sh-4.2$ ./testapp
Signature verified ok
```

## Future Plans
* Allow for static linking so that binary can be executed on Linux host outside of container
* Include Bitcoin c++ libraries in container
* Include Java dependencies
* Reusable scripts that perform key cryptopraphic functions provided by Nakasendo
* Shrink container size... 2 gigs is NOT ideal!!

# THANK YOU!
Huge thank you to nChain for allowing me to play with these libraries early. So much exciting potential moving forward and I would love to foster a community of developers sharing their new found use cases with these libraries. Any contributions are welcome!
