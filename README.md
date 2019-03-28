# selchangechecker
Keep track CLIPBOARD/PRIMARY/SECONDARY selections changes and run `callback` if it
occurs.

All code have been imported from [autocutsel][] except the one line
```c
system(options.callback_path);
```
I use this utility paired with `notitrans` shell script for invoke
X Windows notification message containing word translation when
select the word (e.g. in browser).
```
selchangechecker -s PRIMARY -p 100 -buttonup -c "notitrans.sh"
```

[autocutsel]: https://github.com/sigmike/autocutsel
