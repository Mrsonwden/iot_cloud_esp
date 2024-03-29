Manufacturing Utility
=====================
 

Introduction
----------------

This utility is designed to create per device instances factory nvs partition images for mass manufacturing purposes.   
These images are created from user provided configuration and values csv files.   
This utility only creates the manufacturing binary images and you can choose to use esptool.py or Windows based flash programming utility or direct flash programming to program these images at the time of manufacturing. 

Prerequisites
------------------

**This utility is dependent on the esp-idf nvs partition utility.**

* Operating System requirements:  
	-	Linux / MacOS / Windows (standard distributions)   

* The following packages are needed for using this utility:   
	-	Python version: 2.7 (minimum) is required.
        - Link to install python: <https://www.python.org/downloads/>

.. note:: Make sure the python path is set in the PATH environment variable before using this utility.

Make sure to include packages from `requirement.txt` in top level IDF directory.

Workflow
-----------

.. blockdiag:: 

    blockdiag {
    A [label = "CSV Configuration file"];
    B [label = "Master CSV Values file"];
    C [label = "Binary files", stacked];

    A -- B -> C
    }


CSV Configuration File:
------------------------

This file contains the configuration of the device to be manufactured.
  
The data in configuration file **must** have the following format (`REPEAT` tag is optional):: 

       name1,namespace,	   <-- First entry should be of type "namespace"
       key1,type1,encoding1
       key2,type2,encoding2,REPEAT
       name2,namespace,	   
       key3,type3,encoding3
       key4,type4,encoding4

.. note:: First entry in this file should always be ``namespace`` entry.  

Each row should have these 3 parameters: ``key,type,encoding`` separated by comma.
If ``REPEAT`` tag is present, the value corresponding to this key in the Master CSV Values File will be the same for all devices.

*Please refer to README of nvs_partition utility for detailed description of each parameter.*

Below is a sample example of such a configuration file::


	app,namespace,
	firmware_key,data,hex2bin
	serial_no,data,string,REPEAT
	device_no,data,i32


.. note:: Make sure there are no spaces before and after ',' in the configuration file.

Master CSV Values File:
------------------------

This file contains details of the device to be manufactured. Each row in this file corresponds to a device instance.
  
The data in values file **must** have the following format::

	key1,key2,key3,.....
	value1,value2,value3,.... 

.. note:: First line in this file should always be the ``key`` names. All the keys from the configuration file should be present here in the **same order**. This file can have additional columns(keys) and they will act like metadata and would not be part of final binary files.

Each row should have the ``value`` of the corresponding keys, separated by comma. If key has ``REPEAT`` tag, then its corresponding value **must** be entered in the second line only. Keep the entry empty for this value in the next lines. Below is the description of this parameter:

``value``
	Data value.

Below is a sample example of such a values file::

	id,firmware_key,serial_no,device_no 
	1,1a2b3c4d5e6faabb,A1,101 
	2,1a2b3c4d5e6fccdd,,102 
	3,1a2b3c4d5e6feeff,,103 

.. note:: *If 'REPEAT' tag is present, a new Master CSV Values File is created in the same folder as the input Master CSV File with the values inserted at each line for the key with 'REPEAT' tag.* 

.. note:: *Intermediate CSV files are created by this utility which are input to the nvs partition utility to generate the binary files.*

The format of this intermediate csv file will be::

	key,type,encoding,value
	key,namespace, ,
	key1,type1,encoding1,value1
	key2,type2,encoding2,value2

.. note:: An intermediate csv file will be created for each device instance. 

Running the utility
----------------------

The mfg\_gen.py utility is using the generated CSV Configuration file and Master CSV Values file and is generating per device instance factory images. 

*Sample CSV Configuration file and Master CSV Values file is provided with this utility.* 

**Usage**::
    
    $ ./mfg_gen.py [-h] [--conf CONFIG_FILE] [--values VALUES_FILE]
                    [--prefix PREFIX] [--fileid FILEID] [--outdir OUTDIR]
                    [--size PART_SIZE] [--version {v1,v2}]
                    [--keygen {true,false}] [--encrypt {true,false}]
                    [--keyfile KEYFILE]


+------------------------+----------------------------------------------------------------------------------------------+
|   Arguments            |                                     Description                                              |                                   
+========================+==============================================================================================+
| --conf CONFIG_FILE     |  the input configuration csv file                                                            |
+------------------------+----------------------------------------------------------------------------------------------+
| --values VALUES_FILE   |  the input values csv file                                                                   |
+------------------------+----------------------------------------------------------------------------------------------+
| --prefix PREFIX        |  the unique name as each filename prefix                                                     |
+------------------------+----------------------------------------------------------------------------------------------+
| --fileid FILEID        |  the unique file identifier(any key in values file)                                          |
|                        |  as each filename suffix (Default: numeric value(1,2,3...))                                  |
+------------------------+----------------------------------------------------------------------------------------------+
| --outdir OUTDIR        | the output directory to store the files created (Default: current directory)                 |
+------------------------+----------------------------------------------------------------------------------------------+
| --size PART_SIZE       |  Size of NVS Partition in bytes (must be multiple of 4096)                                   |
+------------------------+----------------------------------------------------------------------------------------------+
| --version {v1,v2}      | Set version. Default: v2                                                                     |
+------------------------+----------------------------------------------------------------------------------------------+
| --keygen {true,false}  | Generate keys for encryption.                                                                |
|                        | Default: false                                                                               |
+------------------------+----------------------------------------------------------------------------------------------+
| --encrypt {true,false} | Set encryption mode. Default: false                                                          |
+------------------------+----------------------------------------------------------------------------------------------+
| --keyfile KEYFILE      | File having key for encryption (Applicable only if encryption mode is true)                  |
+------------------------+----------------------------------------------------------------------------------------------+

*You can use the below commands to run this utility with the sample files provided*::
   
    $ ./mfg_gen.py --conf samples/sample_config.csv --values samples/sample_values_singlepage_blob.csv --prefix Fan --size 0x3000

    $ ./mfg_gen.py --conf samples/sample_config.csv --values samples/sample_values_multipage_blob.csv --prefix Fan --size 0x4000
    
.. note:: When you use this utility to generate per device instance factory images --conf, --values, --prefix and --size arguments are mandatory.
    
    $ ./mfg_gen.py --conf samples/sample_config.csv --values samples/sample_values_singlepage_blob.csv --prefix Fan --size 0x3000 --outdir tmp

.. note:: The --outdir directory is created if not present. 

.. note:: The file path given in the ``file`` type in the values file is expected to be relative to the current directory from which you are running the utility.
    
    $ ./mfg_gen.py --conf samples/sample_config.csv --values samples/sample_values_singlepage_blob.csv --prefix Fan --size 0x3000 --encrypt true --keygen true

.. note:: ``keys/`` directory is generated with the encryption keys filename of the form ``prefix-fileid-keys.bin``.
    
*You can also run the below command to use the utility to* **only** *generate encryption keys binary file ( following example 'keys/' directory is created in current path), which can further be used to encrypt per device instance factory images*::
    
    $ ./mfg_gen.py --keygen true
    
    $ ./mfg_gen.py --keygen true --keyfile encr_keys.bin

.. note:: When running utility to generate only ``keys``, if --keyfile is given it will generate encryption keys with filename given in --keyfile argument.

.. note:: When you use this utility to generate only encryption keys --keygen argument is mandatory.

.. note:: The default numeric value: 1,2,3... of ``fileid`` argument, corresponds to each row having device instance values in master csv values file.

.. note:: ``bin/`` **and** ``csv/`` **sub-directories are created in the** ``outdir`` **directory specified while running this utility. The binary files generated will be stored in** ``bin/`` **and the intermediate csv files generated will be stored in** ``csv/``.

.. note:: Comments are supported in input config csv file only.
