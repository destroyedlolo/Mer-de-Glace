# Mer-de-Glace
CLI oriented simple and lightweight archiving solution

:warning: **Mer-de-Glace** is still in it's very early stage. Documentation will be improved when a stable version will be released

## Dependency
* OpenSSL

## ToDo list
This is the list of identified tasks/behaviors. 
- *data management*
   - [X] Recursively scan a directory with MD5 checksum (v0.1)
   - [X] Save / load state	(v0.2)
   - [X] Restrict scanning to a sub directory (v0.3)
   - [X] re-scan and issue a report (v0.4)
   - [ ] Accept a discrepancy

- *interfaces*
  - [ ] demonize
  - [ ] accept commands via a socket (v0.4)
  - [ ] file system notification

- *for the future*
  - [ ] access to remote stats (is it really useful ?)
  - [ ] local configuration file (*à la .access*)
  - [ ] versioning
