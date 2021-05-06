# AppTimeRec
A daemon that tracks for how much time a process runs throughout a day

## Getting Started

Clone this repository on your machine
```
cd
git clone https://github.com/DevTonyShepard/apptimerec.git
```

### Prerequisites

You need to have installed linux build essentials.

```
sudo apt install build-essential
```

### Installing

cd onto the cloned repo
```
cd ~/apptimerec 
```

Build the executable

```
make
```

if you wish, you can either add the apptimerec folder to your PATH or directly put the executable in your /bin/ folder. use the wizard to do this:

```
chmod +x install
./install
```
## Explanation
First you need to configure the daemon
```
sudo apptimerec -u [username] -a [process list separated by space] -s [sample time in seconds]
```
-u option to filter for a specific user's processes
-a the list of the processes executable names to track (use top command and copy the name under COMMAND)
-s how many time the times file need to be updated

then just start the daemon
```
sudo apptimerec
```

to check times use 
```
sudo apptimerec -p
```
## Future developement
Bugfixes and more option

## Built With
* [Clion](https://www.jetbrains.com/clion/) - IDE

## Authors

* **Antonio "Shepard"** - *Information Engineer* - [DevTonyShepard](https://github.com/DevTonyShepard)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.
