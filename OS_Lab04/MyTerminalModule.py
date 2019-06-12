from sys import argv
from subprocess import call
import os

#help message
__HELP__ = '''Help for MyTerminalModule, ver. 0.1

Supported commands:

cp SOURCE DESTINATION
	Copies a SOURCE file to a DESTINATION file
	SOURCE - source file path
	DESTINATION - destination file path

chmod MODE PATH
	Sets access MODE to a specified PATH
	MODE is an octal number containing 3 digits, first one corresponds for owner's rights, second - for group rights, third - for others' rights. 
	Each digit is from 0 to 7 (octal):
	OCT	BIN	Mask	Rights for file		Rights for directory
	0	000	- - -	no rights		no rights
	1	001	- - x	execute			access to files and their attributes
	2	010	- w -	write			no rights
	3	011	- w x	write, execute		all except filenames access
	4	100	r - -	read			read filenames only
	5	101	r - x	read, execute		read filenames and access to files with treir attributes
	6	110	r w -	read, write		read filenames only
	7	111	r w x	full			full
	There can be one more octal digit prior to those three, it can combinate these values:
	1 - sets the sticky bit, 2 - sets the setgid bit, 4 - sets the setuid bit.
	PATH - filepath of file (directory) to change mode of
	
info PATH
	Shows info about a file or directory specified by PATH
	PATH - path to file or directory to show the info about
	
help
--help
	Shows this message
	
When launched directly, if no additional arguments are specified, interactive mode starts. (To quit the interactive mode, type 'quit' or 'exit'.) Otherwise, program will be trying to execute specified command. 
This program may also be used as a module.'''

__INVITE_HELP__ = "Use 'help' command or '--help' key for help info."

def help():
	print(__HELP__)

def cp(origpath, destpath):
	buffer_size = 256
	try:
		with open(origpath, 'r') as orig, open(destpath, 'w') as dest:
			orig.seek(0, os.SEEK_END)
			file_length = orig.tell()
			orig.seek(0, os.SEEK_SET)
			while orig.tell() < file_length:
				dest.write(orig.read(buffer_size))
	except IOError as err:
		print('cp: ' + str(err) + '. Check command arguments, filepaths or file access permissions. ' + __INVITE_HELP__)

def chmod(mode, path):
	try:
		if len(mode) == 0 or len(mode) > 4:
			print('chmod: Incorrect mode "' + mode + '". Should from 1 to 4 digits. ' + __INVITE_HELP__)
		else:
			#interpret mode string as an octal int
			os.chmod(path, int(mode, 8))
	except ValueError as verr:
		print('chmod: Incorrect mode "' + mode + '". Should contain digits from 0 to 7. ' + __INVITE_HELP__)
	except FileNotFoundError as fnferr:
		print('chmod: ' + str(fnferr) + '. Check path argument. ' + __INVITE_HELP__)

def info(path):
	if os.path.exists(path):
		try:
			#get information
			call(['ls', '-alh', path])
		except Exception as ex:
			print('info: ' + str(ex))
	else:
		print("info: No such file or directory: '" + path + "'. Check path argument. " + __INVITE_HELP__)

#one shot, with args
def _runonce(args):
	if len(args) == 0:
		print('No command specified. ' + __INVITE_HELP__)
	elif args[0] == 'help' or args[0] == '--help':
		if len(args) != 1:
			print('help: invalid arguments count (no arguments required).')
		else:
			help()
	elif args[0] == 'cp':
		if len(args) != 3:
			print('cp: invalid arguments count. ' + __INVITE_HELP__)
		else:
			cp(args[1], args[2])
	elif args[0] == 'chmod':
		if len(args) != 3:
			print('chmod: invalid arguments count. ' + __INVITE_HELP__)
		else:
			chmod(args[1], args[2])
	elif args[0] == 'info':
		if len(args) != 2:
			print('info: invalid arguments count. ' + __INVITE_HELP__)
		else:
			info(args[1])	
	else:
		print('Unknown command. ' + __INVITE_HELP__)

#interactive mode
def _interactive():
	print('Interactive mode')
	inp = input('>> ')
	while inp not in ['quit', 'exit']:
		_runonce(inp.split())
		inp = input('>> ')
	print('Exiting interactive mode')

#if running as main program
if __name__ == '__main__':	
	if len(argv) == 1:
		# only script name specified
		_interactive()
	else:
		# with args
		_runonce(argv[1:])
