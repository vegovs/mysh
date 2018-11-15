# mysh
A very unsafe, crude and simple shell implemented for educational purpose in coherence with  [INF1060](https://www.uio.no/studier/emner/matnat/ifi/INF1060/). Some of the implementations were done in a manner to complete a course task, such as memory handling with datablocks and bitmap for the command-history.

## Install
The shell is written in C and will compile with the help of a makefile. A 'obj' folder to place the .o files is required to compile. The makefile i located in the src folder.

```
mkdir obj
```

### Normal mode:
Compile from the src folder: 
```bash
make
```

### Debug mode:
Prints various debug information.
```bash
make debug
```

### Cleanup:
Removes .o files.
```bash
make clean
```
## Supported commands: 
* Builtins: 
	* 'quit': Quits mysh
	* 'type': Returns command type
	* 'h': Print command/execution history.
	* 'h i': Run command 'i'
	* 'h -d i': Delete history input 'i'
	* 'jobs': List all running jobs
	* 'kill pid': Kill job
* Run programs in background using '&'.
	* Applications requiring TERM and DISPLAY variables set is **NOT** supported.
* Exit using 'Ctrl-D'

## Examples: 
### DEBUG printing.
```bash
vegarbov@mysh 0> pwd
DEBUG: Read: pwd
```

```bash
vegarbov@mysh 0> hello darknes my old friend
DEBUG: Read: hello darknes my old friend
...
OTHER DEBUG INFO
...
DEBUG: Number of tokens saved to saveptr: 5
DEBUG: [0]: hello
DEBUG: [1]: darknes
DEBUG: [2]: my
DEBUG: [3]: old
DEBUG: [4]: friend
```

### CTRL-D: 
```bash
vegarbov@mysh 1>
CTRL-D caught, exiting mysh..
```
### Executing commands
```bash
vegarbov@mysh 0> xyz
mysh: xyz: command not found
```

### Background flag
```bash
vegarbov@mysh 1> vim &
[1] 55929
vegarbov@mysh 2>
```

### `type`
```bash
vegarbov@mysh 2> type h
h is a shell builtin
```

### `h`
```bash
vegarbov@mysh 4> h
History list of the last 5 commands:
  5: h
  4: pwd
  3: cd
  2: ls
  1: h
```

## Architecture
| Fil      | Beskrivelse                                                                             |
|----------|-----------------------------------------------------------------------------------------|
| mysh.c   | Main shell functions|
| mysh.h   | Header file for the entire project|
| bi.c     | All built in functions|
| bm.c     | Bitmap functions|
| mdll.c   | Metadata linked list functions for history handling|
| makefile | make|
