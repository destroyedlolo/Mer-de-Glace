<p align="center">
<img src="https://img.shields.io/github/last-commit/destroyedlolo/Mer-de-Glace.svg?style=for-the-badge" /><br>
(no commit for long time doesn't mean this project is stalling but reached a stable state and I switched to others activities :relaxed:)
<hr/>

# Mer-de-Glace

When talking about ultra-long-term storage, data **integrity quickly** becomes a challenge.
Software bugs, human errors, hardware failures are the most obvious causes of data loss... but sometimes data degrades "on its own", with no visible external cause. Phenomenon known as [data decay, data rot or bit rot](https://en.wikipedia.org/wiki/Data_degradation).

A common strategy is to multiply an odd number of backups, compare them and apply a rule where "*the majority gets the vote*". But it's leading to a costly and slow solution.

**Mer-de-Glace** stores an electronic signature when a file is created. 
The user can ensure the sanity of its data by comparing this archived signature with the current state. 

As a consequence, we are only needing replicas to overcome the failures of the master and not anymore to check data integrity.

> [!TIP]
> **Use Cases** directory for comprehensive examples and use cases.

## Installation from source

**Mer-de-Glace** is a command-line tool targeting to run on slow headless machines : my goal was to recycle obsolete boxes as backup servers. The drawback, unfortunately, mainstream Linux distributions are dropping such obsolete pieces of hardware, leading to compilation problems as **Mer-De-Glace requires a C++20 compliant compiler**.

But alternatives exist :
* some distributions like [TinyCoreLinux](http://www.tinycorelinux.net/) are still supporting old hardware (32 bits, low memory, low processor power). However, before attempting to install from source, check that a binary package does not exist (*I will make one for TinyCoreLinux x86-32b*).
* To the extreme, **Mer-De-Glace** binary can be installed only on one machine : states will be calculated remotely (as explained in "use cases").

### Dependency

* **OpenSSL** (development version)
* **C++-20** compliant tool chain

### compilation

* From a scratch directory clone Mer-De-Glace repository
```
git clone https://github.com/destroyedlolo/Mer-de-Glace.git
cd Mer-de-Glace
make
```

You will get 2 binaries :
* **MerDeGlaced** is the master daemon managing data
* **MdG** command line tool

## Typical usage

### 1- edit configuration file (/usr/local/etc/MerDeGlace.conf by default)
With :
* `rootDirectory=` the root directory of document to track
* `DBFile=` where the state backup is stored

> [!TIP]
> Different kinds of data (photos, music, films) ? Run a **MerDeGlaced** for each of them using a customized configuration file with dedicated rootDirectory, DBFile and rendez-vous.

### 2- Start MerDeGlaced

`MerDeGlaced &`

> [!NOTE]
> Notez-bien : loading an existing backup for a large amount of data may be long. Add verbosity **-v** to know when the application is ready. Alternatively, the "*rendez-vous*" socket is created only when the daemon is ready.

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

* If it's the initial scan, it will report all files as [Created] : you're starting from an empty database, and all files seems new.
* If it's not the initial scan, each discrepancy needs to be investigated : `accept` those legitimate.

> [!TIP]
> `Accept`ing deletion of a directory will commit as well the deletion of all it sub objects.

> [!CAUTION]
> Notez-bien : 
> - don't forget to `save` the state after validating all the discrepancies, otherwise they will reappear when the daemon is restarted.
>- it's not possible to validate checksum issues : they are highlighting potential hardware problems leading to severe data loss.

#### if needed, save the new state

`./MdG save`

### 4- additionally, Mer-De-Glace can hilight duplication

> [!CAUTION]
> Notez-bien : it's about comparing numerical signatures, not the files themselves. It's up to **YOU** to decide if some cleaning is needed or not.

#### check for duplication

```
./MdG -f ~/Config/Musiques.mdg duplicate
Potential duplicate found :
  /mnt/sda4/Musiques/Noir Désir/Noir Désir - 1993 - Tostaky/03 - Oublié.mp3
  /mnt/sda4/Musiques/Noir Desir/1993 - Tostaky/03 - Oublie.mp3
Potential duplicate found :
  /mnt/sda4/Musiques/Noir Désir/Noir Désir - 1994 - Dies Irae/11 - It Spurts.mp3
  /mnt/sda4/Musiques/Noir Desir/1994 - Dies Irae/11 - It Spurts.mp3
Potential duplicate found :
  /mnt/sda4/Musiques/Noir Désir/Noir Désir - 2001 - Des Visages Des Figures/Noir Désir - 2001 - Des Visages Des Figures - Back.jpg
  /mnt/sda4/Musiques/Noir Desir/2001 - Des Visages Des Figures/Noir Désir - 2001 - Des Visages Des Figures - Back.jpg
...
```
#### Do needed cleaning

I made a mistake by converting twice my CD to MP3, using a different naming convention : I have to delete the ".../Noir Désir" directory (using shell's `rm -rf`, a graphical interface or whatever).

#### re-scan to detect deletion

In order to speed up the operation, `restrict` to the directory changed.
```
./MdG -f ~/Config/Musiques.mdg restrict "/mnt/sda4/Musiques/Noir Désir/"
```

<!-- As per v0.9, it's not anymore needed
`RESET` the in memory state.
```
$ ./MdG -f ~/Config/Musiques.mdg RESET
*I* State reseted
```
-->

and finally, launch a new scan.
```
./MdG -f ~/Config/Musiques.mdg scan
```

#### accept deletion

```
./MdG -f ~/Config/Musiques.mdg report
[D][Deleted]	/mnt/sda4/Musiques/Noir Désir
[D][Deleted]	/mnt/sda4/Musiques/Noir Désir/Noir Désir - 1994 - Dies Irae
[F][Deleted]	/mnt/sda4/Musiques/Noir Désir/Noir Désir - 1994 - Dies Irae/13 - The Holy Economic War.mp3
[F][Deleted]	/mnt/sda4/Musiques/Noir Désir/Noir Désir - 1994 - Dies Irae/21 - I Want You (She'S So Heavy).mp3
...
```
```
./MdG -f ~/Config/Musiques.mdg accept '/mnt/sda4/Musiques/Noir Désir'
```

#### save the changes
```
./MdG -f ~/Config/Musiques.mdg save
```

## MdG, the Command line client
**MdG** is the command line client to communication with **MerDeGlaced** daemon

`./MdG [-opt] command [arguments ...]`

with `./MdG -h` to get a list of supported options. `./MdG help` to get the list of commands known by the daemon.

### scripting
**MdG** issues following return codes :

-   0 : everything goes right
- 100 : something has been found by `report` or `duplicate` commands
-   1 : technical issue

So following code can be used for automation needs

```shell
if ./MdG -f tst.conf report > /dev/null
then
    echo "everything is fine"
else
    if [[ $? -eq 100 ]]
    then
        echo "something found"
    else
        echo "technical issue"
    fi
fi
```

### Notes about commands

#### report
Reports of state discrepancies as :
```console
[D][Created]	/home/laurent/Images/Brute/_AArchiver/test/new
[F][Changed]	/home/laurent/Images/Brute/_AArchiver/test/toto
```
With
- the object type can be `[D]` for a directory or `[F]` for a file.
- `[Created]`, `[Deleted]`, `[Changed]` as the names said
- `[Replica only]`, `[Master only]`, `[Discrepancy]` while comparing an alternate root
- `[Bad CS]` the checksum doesn't correspond to the signature **highlighting server corruption**
- `[ERROR]` an issue has been encountered while processing (typically, a file that is not readable)

#### save

Mer-De-Glace maintains in memory files' state. You can (have) to `save` it to retrieve it at restart and check if data remains safe.

> [!NOTE]  
> Notez-bien :
> - When the state is saved using the `save` command, all files/directories *creations* are de facto accepted.
> - *modification* and *deletion* are pending as they may highlight a storage issue.

#### RESET / RAZ
**RESET** command will reset the state of each file/directory as *clean* item. 

As discrepancies will be lost, is command is **dangerous** and need to be used with caution !

> [!WARNING]
> After a **RESET**, in memory state is not any more consistent until the next scan, and all identified discrepancies are lost. In case of doubt, **restart *MerDeGlaced* without saving** and then launch a scan : it will reset data as per the real situation.

#### RECS
**Mer-de-Glace** keeps internal checksums to ensure in memory state as well as backup ones are not corrupted.
On **very rare** occasions, rebuilding them is needed : it's the goal of **RECS** (for *recalculate checksum*).

> [!CAUTION]
> This command is **very dangerous** as a checksum discrepancy is a proof of something going very bad (a disk being corrupted, a memory fault, hardware failure, ...). Consequently, this command is allowed ONLY if the daemon has been started in debug mode.

## ToDo list
This is the list of identified tasks/behaviors. 
- *data management*
   - [X] Recursively scan a directory with MD5 checksum (v0.1)
   - [X] smart status reset before scanning to avoid usage of explicite `RESET` command (v0.9)
   - [X] Save / load state	(v0.2)
   - [X] Restrict scanning to a sub directory (v0.3)
   - [X] re-scan and issue a report (v0.4)
   - [X] Accept a discrepancy (v0.6)
   - [X] Guess duplicate entries (v0.8)
   - [X] verify in memory and backup integrity (v0.7)
   - [X] Can use alternate root (v0.8)

- *interfaces*
  - [X] accept commands via a socket (v0.4)
  - [ ] grouped acceptation (i.e. :  accept all deletions, all creations, all modifications, ...). Restrictions apply.
  - [X] daemonize (avoid as much as possible exiting in case of issue) (v0.11)
  - [X] Command line tool (v0.5)
  - [X] long standing commands are aborted when client connection is lost (v0.11)
  - [ ] Shell file name completion
  - [X] Generate return code to make automatic scripts easier (v0.10)

- *for the future*
  - [ ] access to remote stats (is it really useful ? Mounting remote FS and using alternate root is already doing the job, see Use Cases)
  - [ ] local configuration file (*à la .access*)
  - [ ] versioning
  - [ ] Daemon dashboard : GUI, replicas auto discovering, status of each replica, central place for replications management  ...

- *Questionable*
Stuffs I'm thinking about but having big impacts, imply issues or potentially not useful.
  - [ ] Asynchronous action :arrow_right: Will require deep architecture review and makes the source code more complex (semaphores, how to handle a file processing if the user already asks for its deletion, ...). And in any case, as the disk IO is definitively a bottleneck, is it really useful ?
  - [ ] file system notification :arrow_right: primary test highlights the notification is not fully reliable. As processing a file may be long, asynchronous actions would be needed and probably an action queue as well. Frankly speaking, it will also encourage the laziness of users, leading to less frequent full scan.

---

# Licencing and contributing

**Mer-de-Glace** is covered by [Creative Commons-BY-NC](http://creativecommons.org/licenses/by-nc/3.0/) preventing *vampires* to abuse open source developers kindness : Please raise a ticket if you want to integrate it in a commercial product.

**Feel free to participate** : code improvements, new features implementation, beer to developers, gifts, thanks messages :clap: ... Participations help to make projects alive.
