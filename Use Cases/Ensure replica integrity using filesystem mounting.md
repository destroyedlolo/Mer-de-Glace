Secured backups are requiring multiple replicas of the same set of data.

The way to ensure that each and every replica is safe is explained in this small documentation.

:bulb: The methode explained here is usefull when **Mer-De-Glace** can't be installed on a replicat 
(for exemple, when the it's compilation chain is too old to supprt C++-20).
But the prefered methode is to copy the state on the replica as explained in "*Ensure replica integrity using state replication.md*"

## Mount the remote filesystem
### create a mount point
```
mkdir /tmp/bpi
```
### Mount using sshfs

"SSHFS allows you to mount a remote filesystem using SFTP" using FUSE, so by normal users.

```
sshfs $( avahi-resolve-host-name -n4 bpi.local | cut -f2 ):Musiques /tmp/bpi/
```
Note : `avahi-resolve-host-name` bloc is on the play only because my machines are managed by **ZeroConf/avahi** and unfortunately, 
avahi name resolution is not working on my busybox based TinyCoreLinux. On machines with full avahi support, the command would have been :

```
scp bpi.local:Musiques /tmp/bpi/
```

### Mer De Glace
#### Start the deamon if needed
```
./MerDeGlaced -f ~/Config/Musiques.mdg
```
### Ensure master integrity
As always, it's better to ensure we are starting from a trusted base. On the master, issue a scan and ask a report for corruption.
```
./MdG -f ~/Config/Musiques.mdg scan
...
./MdG -f ~/Config/Musiques.mdg report
```
 (the report should be empty, if not, treat the discrepancies)

### set alternate root
```
$ ./MdG -f ~/Config/Musiques.mdg altroot /tmp/bpi/
*I* Alternate root set
*W* State reseted
```
Notez-bien : as the warning said, the state is reset ; consequently, the result of the previous scan is lost (on purpose).

### launch a new scan on the remote
```
./MdG -f ~/Config/Musiques.mdg scan
...
./MdG -f ~/Config/Musiques.mdg report
```
