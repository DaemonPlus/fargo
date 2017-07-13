# fargo
Flattens alpha, removes ghosts, optimized.

## Why?

A lot of digital artists either do not flatten their opaque images before
publishing, or paint directly on transparent backgrounds, which leave very small
amounts of semi-transparent pixels in an image designed to be fully opaque.

optipng is a good tool for automatically optimizing images, but it doesn't ever
reduce available data. Specifically, if an image has even one pixel with a
different alpha, the entire image will keep the alpha layer, wasting space.

fargo attempts to remedy this by ignoring the alpha layer, unless there is a
large amount of alpha pixels. This saves a lot of data (15% to 20% normally!),
with no human visible quality loss.

## Building and Installing

Requires [libvips](https://github.com/jcupitt/libvips).

```shell
git clone https://github.com/DaemonLee/fargo
cd fargo
make
sudo make install
```

AUR package for Arch users: [fargo-git](https://aur.archlinux.org/packages/fargo-git/)
