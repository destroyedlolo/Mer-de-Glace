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

## Installation from source

**Mer-de-Glace** is a command-line tool targeting to run on slow headless machines.

My goal when starting this project was to recycle old/slow boxes as backup server. Unfortunately, mainstream Linux distributions are dropping such obsolete hardwares leading to compilation problem as **Mer-De-Glace is requiring C++20 compliant compiler**. 

But alternatives exist :
* some distributions like [TinyCoreLinux](http://www.tinycorelinux.net/) are still supporting old hardware (32 bits, low memory, low processor power). However, before attempting to install from source, check that a binary package does not exist (*I will make one for TinyCoreLinux x86-32b*).
* To the extreme, **Mer-De-Glace** binary can be installed only on one machine : states will be calculated remotely (as explained in "use cases").

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


## MdG, the Command line client
**MdG** is the command line client to communication with **MerDeGlaced** daemon

`./MdG [-opt] command [arguments ...]`

with `./MdG -h` to get list of supported options. `./MdG help` to get the list of commands known by the daemon.

### Notes about commands

#### save

Mer-De-Glace maintains in memory files' state, which will be lost obviously when the server is restarted. It can put a snapshot of this state on disk using `save` command, a snapshot that will be automatically reloaded at daemon start.

Notez-bien :
- When the state is saved, using `save` command, all files/directories *creation* are de facto accepted.
- *modification* and *deletion* still need explicit acceptation (using `accept` command) as they may highlight a storage issue.

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

Retrieve the current status of your monitored directory tree.

It will take a long time, depending on your disk speed, CPU workforce, number and size of files to handle.

#### Check the result

```
./MdG report
[D][Deleted]	/home/laurent/Images/Brute/_AArchiver/test/tst
[F][Deleted]	/home/laurent/Images/Brute/_AArchiver/test/tst/truc
```

* If it's the initial scan, it will report all files as [Created] : you're starting from an empty database and all files seems new.
* If it's not the initial scan, each discrepancy needs to be investigated : `accept` those legitimate.

| :bulb: | `Accept`ing deletion of a directory will commit as well the deletion of all it sub object. |
|-------------|----------------------------|

Notez-bien : 
- don't forget to `save` the state after validating all the discrepancies, otherwise they will reappear when the daemon is restarted.
- it's not possible to validate checksum issues : they are highlighting potential hardware problems leading to severe data loss.

#### if needed, save the new state

`./MdG save`

### 4- additionally, Mer-De-Glace can identify duplication

Notez-bien : it's about comparing numerical signature, not the files themselves. It's up to **YOU** to decide if some cleaning is needed or not.

#### check for duplication

```
./MdG -f ~/Config/Musiques.mdg duplicate
Potential duplicate found :
  /mnt/sda4/Musiques/Noir D??sir/Noir D??sir - 1993 - Tostaky/03 - Oubli??.mp3
  /mnt/sda4/Musiques/Noir Desir/1993 - Tostaky/03 - Oublie.mp3
Potential duplicate found :
  /mnt/sda4/Musiques/Noir D??sir/Noir D??sir - 1994 - Dies Irae/11 - It Spurts.mp3
  /mnt/sda4/Musiques/Noir Desir/1994 - Dies Irae/11 - It Spurts.mp3
Potential duplicate found :
  /mnt/sda4/Musiques/Noir D??sir/Noir D??sir - 2001 - Des Visages Des Figures/Noir D??sir - 2001 - Des Visages Des Figures - Back.jpg
  /mnt/sda4/Musiques/Noir Desir/2001 - Des Visages Des Figures/Noir D??sir - 2001 - Des Visages Des Figures - Back.jpg
...
```
#### Do needed cleaning

I made a mistake by converting twice my CD to MP3, using a different naming convention : I have to delete ".../Noir D??sir" directory (using shell's `rm -f`, a graphical interface or whatever).

#### re-scan to detect deletion

In order to speed up the operation, `restrict` to the directory changed.
```
./MdG -f ~/Config/Musiques.mdg restrict "/mnt/sda4/Musiques/Noir D??sir/"
```

`RESET` the in memory state.
```
$ ./MdG -f ~/Config/Musiques.mdg RESET
*I* State reseted
```

and finaly, launch a new scan.
```
./MdG -f ~/Config/Musiques.mdg scan
```

#### accept deletion

```
./MdG -f ~/Config/Musiques.mdg report
[D][Deleted]	/mnt/sda4/Musiques/Noir D??sir
[D][Deleted]	/mnt/sda4/Musiques/Noir D??sir/Noir D??sir - 1994 - Dies Irae
[F][Deleted]	/mnt/sda4/Musiques/Noir D??sir/Noir D??sir - 1994 - Dies Irae/13 - The Holy Economic War.mp3
[F][Deleted]	/mnt/sda4/Musiques/Noir D??sir/Noir D??sir - 1994 - Dies Irae/21 - I Want You (She'S So Heavy).mp3
...
```
```
./MdG -f ~/Config/Musiques.mdg accept '/mnt/sda4/Musiques/Noir D??sir'
```

#### save the changes
```
./MdG -f ~/Config/Musiques.mdg save
```

## ToDo list
This is the list of identified tasks/behaviors. 
- *data management*
   - [X] Recursively scan a directory with MD5 checksum (v0.1)
   - [ ] smart status reset before scanning to avoir usage of explicite `RESET` command
   - [X] Save / load state	(v0.2)
   - [X] Restrict scanning to a sub directory (v0.3)
   - [X] re-scan and issue a report (v0.4)
   - [X] Accept a discrepancy (v0.6)
   - [X] Guess duplicate entries (v0.8)
   - [X] verify in memory and backup integrity (v0.7)
   - [ ] Can use alternate root

- *interfaces*
  - [X] accept commands via a socket (v0.4)
  - [ ] daemonize (avoid as much as possible exiting in case of issue)
  - [X] Command line tool (v0.5)
  - [ ] long standing commands are aborted when client connection is lost
  - [ ] file system notification
  - [ ] Shell file name completion

- *for the future*
  - [ ] access to remote stats (is it really useful ?)
  - [ ] local configuration file (*?? la .access*)
  - [ ] versioning
