<p align="center">
<img src="https://img.shields.io/github/last-commit/destroyedlolo/Mer-de-Glace.svg?style=for-the-badge" />
<hr/>

# Mer-de-Glace
Simple and lightweight archiving integrity solution

| :exclamation: | **Notez-bien :** **Mer-de-Glace** is still in it's very early stage. Documentation will be improved when a stable version will be released  |
|-------------|----------------------------|

## Dependency
* **OpenSSL**

## Command line
**MdG** is the command line client to communication with **MerDeGlaced** daemon

`./MdG [-opt] command [arguments ...]`

with `./MdG -h` to get list of supported options. `./MdG help` to get the list of known command.

### Notes about commands
#### save
When the state is saved, using `save` command, all files/directories `creation` are *de facto* accepted.
`modification` and `deletion` still need explicit acceptation (using accept `command`) as it may highlight a storage issue.

#### RESET / RAZ
**RESET** command will reset the state of each file/directory and is only aimed to be used before a **scan**.
Typical usage : a lot of not committed modifications as been made, leading to a mess in the in-memory database.

As a rule of thumb, it's **always** better to avoid usage of **RESET** but commit frequently changes.

| :warning: | This command is **very dangerous**. In case of doubte, **restart *MerDeGlaced* without saving** and then launch a scan : it will reset data as per the real situation |
|-------------|----------------------------|

## ToDo list
This is the list of identified tasks/behaviors. 
- *data management*
   - [X] Recursively scan a directory with MD5 checksum (v0.1)
   - [X] Save / load state	(v0.2)
   - [X] Restrict scanning to a sub directory (v0.3)
   - [X] re-scan and issue a report (v0.4)
   - [ ] Accept a discrepancy

- *interfaces*
  - [X] accept commands via a socket (v0.4)
  - [ ] demonize (avoid as much as possible exiting in case of issue)
  - [X] Command line tool (v0.5)
  - [ ] long standing commands are aborted when client connection is lost
  - [ ] file system notification

- *for the future*
  - [ ] access to remote stats (is it really useful ?)
  - [ ] local configuration file (*à la .access*)
  - [ ] versioning
