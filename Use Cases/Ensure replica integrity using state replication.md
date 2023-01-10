Secured backups are requiring multiple replicas of the same set of data.

The way to ensure that each and every replica is safe is explained in this small documentation.

:bulb: This is **the preferred method** versus mounting a remote file system (see "*Ensure replica integrity using filesystem mounting*") example : here, the state will be backup on all server.

## Ensure master integrity
As always, it's better to ensure we are starting from a trusted base. On the master, issue a scan and ask a report for corruption.

## Copy master state to the replica

```
scp $( avahi-resolve-host-name -n4 grossvg1.local | cut -f2 ):/mnt/sda5/photos.mdg /mnt/sda5/
xxx@192.168.0.50's password:
photos.mdg                                                                            100% 3120KB  10.2MB/s   00:00
```

Note : `avahi-resolve-host-name` bloc is on the play only because my machines are managed by **ZeroConf/avahi** and unfortunately, 
avahi name resolution is not working on my busybox based TinyCoreLinux. On machines with full avahi support, the command would have been :

```
scp grossvg1.local:/mnt/sda5/photos.mdg /mnt/sda5/
```

:bulb: : the master is not anymore needed, it can be poweroff.

## Launch MerDeGlaced

```
$ ./MerDeGlaced -vf ~/Config/Photos.mdg
Mer de Glace daemon (MerDeGlaced) v0.07
        root directory to scan : '"/mnt/sda5/ArchivesPhotos"'
        Database : '/mnt/sda5/photos.mdg'
        Rendez-vous : '/tmp/Photos.rendez-vous'
*I* Database reloaded
```

## Ask for a new scan
This scan will check replica's files against of the corresponding state on the master.
```
$ ./MdG -f ~/Config/Photos.mdg scan
```

## and then for a report
```
$ ./MdG -f ~/Config/Photos.mdg report
```

If this report is empty, *congratulation* : you have a secure replicat !
