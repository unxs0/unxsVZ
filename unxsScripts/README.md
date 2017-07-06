## unxsScripts

Will start to document what is going on here.

### Basic unxsVZ node job queue configuration

We will need to setup /etc/unxsvz/ and /etc/cron.d/ scripts.

#### Bare minimum

```
[root@unxs1 unxs]# ls -1 /etc/cron.d/unxsvz_*
/etc/cron.d/unxsvz_graphs.cron
/etc/cron.d/unxsvz_main.cron

[root@unxs1 unxs]# ls -1 /etc/unxsvz/*
/etc/unxsvz/datacenter.local.sh
/etc/unxsvz/iptables.local.sh
/etc/unxsvz/mysql.local.sh
/etc/unxsvz/scp.local.sh
/etc/unxsvz/unxsvz.functions.sh
/etc/unxsvz/unxsvz.mail.sh
/etc/unxsvz/zabbix.local.sh
```

#### One node per datacenter will have

```
/etc/cron.d/unxsvz_graphs_datacenter.cron
```

### rrdtool Graphs

We have a collection of scripts and sourced bash config files that take care of
generating (along with the unxsVZ DB) datacenter, node and container traffic graphs.

The graphs are stored on all nodes for HA and DR, and also some are copied to the main unxsVZ management container.

The tConfiguration table also contains configuration name/value pairs that affect how unxsVZ.cgi displays these graphics.

```
cTrafficDir:https://node1.yourisp.net:9333/traffic
London32A-cDatacenterTrafficPNG:/traffic/EquinixLon32A.png
```
