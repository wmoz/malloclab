
# Heap Inspector

The specification for Project 3 is on the course website. This README describes the specification for a new tool that can aid in the development of the malloclab project.

The heap inspector is a tool written in Python to catch common errors in execution right as they happen. Common errors may be corruptions to the link list elements, library code returning pointers to already allocated regions, among others.

In addition, this tool can help students visualize memory writes in the heap right as they happen, to better understand what the heap looks like, and how the common malloclab errors manifest themselves.

## Getting Started
Assuming ``~cs3214/bin`` is in your ``$PATH`` (which you should've done for the earlier projects), running the tool is as simple as running ``heapinspector.sh`` in the directory containing your ``mm.c`` file. There are various command line options you should be aware of.

Various flags can be passed to the heap inspector. These are as follows:

### ``-s``, ``--same-heap``
When specifying this option, ``mmap`` will be used internally to create the initial heap. This has the advantage of consistency, because heap addresses will not be randomized.

### ``-d``, ``--disable-free``
Disable the check for integrity of free lists. This may make the program run significantly faster, but has the disadvantage of not being able to catch free list corruptions as they happen.

### ``-r``, ``--rebuild``
Force a rebuild of the malloclab project. By default, the project will only run ``make``, but this will also run ``make clean``.

### ``-b BYTES``, ``--bytes BYTES``
Specify the amount of heap bytes to display in the inspector tool. By default, this is set to 128, but specifying this option will set it to BYTES.

Note: your terminal window must be large enough to display this many bytes.

## Heap Inspector REPL
When launching the ``heapinspector.sh`` file with any arguments you may want, you will be greeted with a prompt that you can type various commands in. Commands can be entered by their first letter to save time. These commands are as follows:

### ``help``
Open the help menu.

### ``allocate POINTER SIZE``
Allocates ``SIZE`` bytes and associates the memory address with ``POINTER``. Think of ``POINTER`` as the human-readable "variable" representation of the address returned from a successful call to ``malloc``.

``POINTER`` is a an alphanumeric string (which can also include hyphens, underscores, and periods).

Example: ``allocate 1 1024``

### ``free POINTER``

Free the memory associated with ``POINTER``.

``POINTER`` is a an alphanumeric string (which can also include hyphens, underscores, and periods). Please do not use the memory address of the allocated region.

### ``reallocate POINTER SIZE``
Reallocates ``SIZE`` bytes and associates the memory address with ``POINTER``. Think of ``POINTER`` as the human-readable "variable" representation of the address returned from a successful call to ``realloc``.

``POINTER`` is a an alphanumeric string (which can also include hyphens, underscores, and periods).

Example: ``reallocate abc 1024``

### ``trace``
Execute a trace file. This will open up a dialog to select a trace file. Students can even chose to create their own trace files in their ``traces`` directory which will appear in the input.

When executing a trace file, the student can use keyboard controls to navigate through each trace. They can also exit the trace file, execute other tasks in the inspector, and come back to the trace.

### ``dump``, ``dump LIST``
Executing ``dump`` without any arguments will simply dump information about the free lists, including where they are located in memory, the symbol associated with the free list, and how many elements are contained within them.

When specifying an argument here (where ``LIST`` is the list number, as displayed in the leftmost column of the free list dump), pointers to each ``list_elem`` that are in this linked list will be displayed.

### ``live``
This will print a list of all of the live allocations at this point in execution, including the human-readable representation of that pointer, the memory address, and the size.

### ``gdb``
This will execute ``gdb``. Note: this mode is most useful when the heap inspector encounters a situation in which the program may crash, so that the user can look at information regarding the execution leading up to the crash.

### ``view``
This will open up the memory viewing tool. An in-depth description on this tool is later in this README.

### quit
Quit the memory inspector

## Memory Viewer
The memory viewer tool is an intuitive interface that allows students to quickly and easily step through changes in their heap implementation.

The heap inspector will instrument the student's malloclab implementation and identify every memory write to the heap. This information is not only used to identify invalid writes and free list corruptions, but can allow the capability of stepping forward and backwards in different heap actions and view the memory at specific addresses.

While executing the memory viewer, the student can access a help menu (by pressing ``h``) to learn how the memory viewer works.