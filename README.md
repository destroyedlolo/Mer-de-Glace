<p align="center">
<img src="https://img.shields.io/github/last-commit/destroyedlolo/Mer-de-Glace.svg?style=for-the-badge" />
<hr/>

# Mer-de-Glace

When talking about ultra-long-term storage, data **integrity quickly** becomes a challenge.
Software bugs, human errors, hardware failures are the most obvious causes of data loss... but sometimes data degrades "on their own", with no visible external cause. Phenomenon known as [data decay, data rot or bit rot](https://en.wikipedia.org/wiki/Data_degradation).

A common strategy is to multiply an odd number of backups, compare them and apply a rule where "*the majority gets the vote*". But it's leading to a costly and slow solution.

**Mer-de-Glace** stores an electronic signature when a file is created. 
The user can ensure the sanity of its data by comparing this archived signature with the current state. 

As a consequence, we are needing replicas only to overcome the failures of the master and not anymore to check data integrity (in my setup, I'm using a quite fast machine as a master but replicas runs on old and slow hardware).

| :exclamation: | **Notez-bien :** **Mer-de-Glace** is still in *beta* stage. Documentation will be improved when a stable version will be released  |
|-------------|----------------------------|

See **Use Cases** directory for comprehensive examples and use cases.

## Installation

**Mer-de-Glace** is a command-line tool targeting to run on slow headless machines.

If like me, the goal is to recycle old boxes as backup servers, such hardware is probably no longer supported by recent mainstream Linux distributions : this may be a problem to compile Mer-De-Glace as a C++20 compiler is required. 

Alternatives exist :
* some distributions like [TinyCoreLinux](http://www.tinycorelinux.net/) are still supporting quite old hardware
* To the extreme, **Mer-De-Glace** binary can be installed only on one machine : states can be calculated remotely (see use cases).

### Dependency

* **OpenSSL** (development version)
* C++-20 compliant tool chain

### compilation

* go to a scratch directory
* clone Mer-De-Glace repository
```
git clone https://github.com/destroyedlolo/Mer-de-Glace.git
```
* from the newly created directory enter
```
make
```

You will get 2 binaries :
* **MerDeGlaced** is the master daemon managing data
* **MdG** command line tool


## About Command line
**MdG** is the command line client to communication with **MerDeGlaced** daemon

`./MdG [-opt] command [arguments ...]`

with `./MdG -h` to get list of supported options. `./MdG help` to get the list of known command.

### Notes about commands
#### save
When the state is saved, using `save` command, all files/directories *creation* are de facto accepted.

*modification* and *deletion* still need explicit acceptation (using `accept` command) as they may highlight a storage issue.

#### RESET / RAZ
**RESET** command will reset the state of each file/directory and is only aimed to be used before a **scan**.
Typical usage : a lot of not committed modifications as been made, leading to a mess in the in-memory database.

As a rule of thumb, it's **always** better to issue a `report` and handle discrepancies found before using this command.

| :eyes: | After a **RESET**, in memory state is not anymore consistent until the next scan and all identified discrepancies are lost. In case of doubt, **restart *MerDeGlaced* without saving** and then launch a scan : it will reset data as per the real situation |
|-------------|----------------------------|

#### RECS
**Mer-de-Glace** keeps internal checksums to ensure in memory state as well as backup ones are not corrupted.
In **very rare** occasion, rebuilding them is needed : it's the goal of **RECS** (for *recalculate checksum*).

| :warning: | This command is **very dangerous** as checksum discrepancy is a proof of something going very bad (disk being corrupted, memory fault, hardware failure, ...). Consequently, this command is allowed ONLY if the daemon as been started in debug mode. |
|-------------|----------------------------|

## Typical usage

### 1- edit configuration file (/usr/local/etc/MerDeGlace.conf by default)
With :
* `rootDirectory=` the root directory of document to track
* `DBFile=` where the state backup is stored

### 2- Start MerDeGlaced

`MerDeGlaced &`

Notez-bien : loading an existing backup may be long. Add verbosity **-v** to know when application is ready.

### 3- use MdG to communicate

#### Launch a new scan

`./MdG scan`

It will take a long time, depending on your disk speed, CPU workforce, number of size of files to handle.

#### Check the result

```
./MdG report
[D][Deleted]	/home/laurent/Images/Brute/_AArchiver/test/tst
[F][Deleted]	/home/laurent/Images/Brute/_AArchiver/test/tst/truc
```

* If it's the initial scan, it will report all files as [Created].
* If it's not the initial scan, each discrepancy needs to be investigated : `accept` those who are legitimate.

| :bulb: | `Accept`ing deletion of a directory will commit the deletion of all it sub object. |
|-------------|----------------------------|

Notez-bien : 
- don't forget to `save` the state after validating all the discrepancies, otherwise they will reappear when the daemon is restarted.
- it's not possible to validate checksum issues : they are highlighting potential hardware problems leading to severe data loss.

#### if needed, save the new state

`./MdG save`

### 4- additionally, Mer-De-Glace can identify duplication

Notez-bien : it is comparing numerical signature. It's up to **YOU** to decide if some cleaning is needed or not.

## ToDo list
This is the list of identified tasks/behaviors. 
- *data management*
   - [X] Recursively scan a directory with MD5 checksum (v0.1)
   - [X] Save / load state	(v0.2)
   - [X] Restrict scanning to a sub directory (v0.3)
   - [X] re-scan and issue a report (v0.4)
   - [X] Accept a discrepancy (v0.6)
   - [X] Guess dupplicate entries (v0.8)
   - [X] verify in memory and backup integrity (v0.7)

- *interfaces*
  - [X] accept commands via a socket (v0.4)
  - [ ] daemonize (avoid as much as possible exiting in case of issue)
  - [X] Command line tool (v0.5)
  - [ ] long standing commands are aborted when client connection is lost
  - [ ] file system notification

- *for the future*
  - [ ] access to remote stats (is it really useful ?)
  - [ ] local configuration file (*à la .access*)
  - [ ] versioning
