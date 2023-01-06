This document explains how to migrate from a pre-v0.07 backup : 
although it probably only usefull to ... myself, it explains as well **how to rebuild checksums**.

| :exclamation: | Needless to remind, except for this kind of migration, it is very strongly not recommended to do so: if your checksums are corrupted, there is a BIG risk in terms of the integrity of the saved data. So you have been warned.  |
|-------------|----------------------------|

## Context

I generated a state backup for my photos from **Mer de Glace v0.04**. 
The concern here is there was no checksum before v0.07 ... and newer versions of **Mer de Glace** are considering such backup as corrupted.

```
$ ./MerDeGlaced -vf ~/Config/Photos.mdg
Mer de Glace daemon (MerDeGlaced) v0.07
        root directory to scan : '"/mnt/sda5/ArchivesPhotos"'
        Database : '/mnt/sda5/photos.mdg'
        Rendez-vous : '/tmp/Photos.rendez-vous'
*F* The backup is corrupted !
```
However, these backups remain sufficient to ensure that the photos are still uncorrupted 
(the checksums are mainly there to ensure the cleanliness of backup themselves).

## Correctives actions

### 1-Backup your backup file

```
$ cp photos.mdg photos.mdg_v0.04
```

### 2-Start Mer De Glace daemon in debug mode
By adding **-d** option (caution, it takes time espacially on slow machine : redirecting the output to /dev/null may spead up).

```
$ ./MerDeGlaced -df ~/Config/Photos.mdg
Mer de Glace daemon (MerDeGlaced) v0.07
        root directory to scan : '"/mnt/sda5/ArchivesPhotos"'
        Database : '/mnt/sda5/photos.mdg'
        Rendez-vous : '/tmp/Photos.rendez-vous'
*D* --> root : /mnt/sda5/ArchivesPhotos
*d* findDir(/mnt/sda5/ArchivesPhotos/Mariage, rec, create)
*d* cur: '"/mnt/sda5/ArchivesPhotos"'
      -> '"/mnt/sda5/ArchivesPhotos/Mariage"'
*d* Create new directory
*d* findDir(Mariage, create)
*E* The backup is corrupted !

...
                        File 'GOPR1447.JPG' : hist:70C60464BFE80EA14E4304C1167DE135 -0/712- act: CS
                        File 'G0051460.JPG' : hist:D0DE124D2C526A40CA1EF19B87CCE0EC -0/76d- act: CS

*W* ***** WARNING WARNING, WARNING *****
*W* Loaded backup is corrupted
*W* ***** WARNING WARNING, WARNING *****

*d* waiting for event
```
### 3-recalculate checksum
Using `RECS` command.

```
$ ./MdG -f ~/Config/Photos.mdg RECS
*W* Checksum recalculated
```

### 4-Save the result
Using `save` command.

```
$ ./MdG -f ~/Config/Photos.mdg RECS
*W* Checksum recalculated
```

### 5-Finally, restart the daemon in normal mode
Restart without **-d** option.
Here, I'll stay in verbose mode as a demonstration, *quiet* mode (without **-d** or **-v**) should be the normal way of working.

```
$ ./MerDeGlaced -vf ~/Config/Photos.mdg
Mer de Glace daemon (MerDeGlaced) v0.07
        root directory to scan : '"/mnt/sda5/ArchivesPhotos"'
        Database : '/mnt/sda5/photos.mdg'
        Rendez-vous : '/tmp/Photos.rendez-vous'
*I* Database reloaded
```
You can saffely remove the "backup of the backup" you made at first.
