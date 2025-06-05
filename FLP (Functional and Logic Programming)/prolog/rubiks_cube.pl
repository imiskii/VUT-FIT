% FLP - Rubik's cube solver
% Michal Ľaš (xlasmi00)
% Date: 11.04.2025
% File: rubiks_cube.pl

% //////////////////////////////////////////////////////////////////////////////////////////////////// %
% ///////////////////////////////// I/O operations and input parsing ///////////////////////////////// %
% //////////////////////////////////////////////////////////////////////////////////////////////////// %

% Note: predicates `read_line`, `isEOFEOL`, `read_lines` and `split_line` were taken from input2.pl by the author Martin Hyrs


/** Reads line from STDIN, end separator is LF or EOF **/
/** cte radky ze standardniho vstupu, konci na LF nebo EOF */
read_line(L,C) :-
	get_char(C),
	(isEOFEOL(C), L = [], !;
		read_line(LL,_),% atom_codes(C,[Cd]),
		[C|LL] = L).


/** Test symbol on EOF or LF **/
/** testuje znak na EOF nebo LF */
isEOFEOL(C) :-
	C == end_of_file;
	(char_code(C,Code), Code==10).


read_lines(Ls) :-
	read_line(L,C),
	( C == end_of_file, Ls = [] ;
	  read_lines(LLs), Ls = [L|LLs]
	).


/** Split line on sublists **/
/** rozdeli radek na podseznamy */
split_line([],[[]]) :- !.
split_line([' '|T], [[]|S1]) :- !, split_line(T,S1).
split_line([32|T], [[]|S1]) :- !, split_line(T,S1).    % aby to fungovalo i s retezcem na miste seznamu
split_line([H|T], [[H|G]|S1]) :- split_line(T,[G|S1]). % G je prvni seznam ze seznamu seznamu G|S1


% Formats the input into the format that is easier to work with.
% The Up side and Down side have good formating, the problem are three middle lines containing rest of the sides.
% These three lines (X,Y,Z) are parsed with `split_line` and formated into rows (indexes 1,2 and 3) of individual sides.
format_cube(
    [
        Up1, Up2, Up3,
        X,Y,Z,
        Down1, Down2, Down3
    ],
    [
        Up1, Up2, Up3,
        Front1, Front2, Front3,
        Right1, Right2, Right3,
        Back1, Back2, Back3,
        Left1, Left2, Left3,
        Down1, Down2, Down3
    ]
):- 
    split_line(X, [Front1, Right1, Back1, Left1]),
    split_line(Y, [Front2, Right2, Back2, Left2]),
    split_line(Z, [Front3, Right3, Back3, Left3]).


% Prints one row of the side of the cube
print_cube_row(Row) :- atom_chars(X, Row), write(X).


% Prints the formated cube on the STDOUT
print_cube(
    [
        Up1, Up2, Up3,
        Front1, Front2, Front3,
        Right1, Right2, Right3,
        Back1, Back2, Back3,
        Left1, Left2, Left3,
        Down1, Down2, Down3
    ]
) :-
    print_cube_row(Up1),nl,
    print_cube_row(Up2),nl,
    print_cube_row(Up3),nl,
    print_cube_row(Front1), print_cube_row([' '|Right1]), print_cube_row([' '|Back1]), print_cube_row([' '|Left1]), nl,
    print_cube_row(Front2), print_cube_row([' '|Right2]), print_cube_row([' '|Back2]), print_cube_row([' '|Left2]), nl,
    print_cube_row(Front3), print_cube_row([' '|Right3]), print_cube_row([' '|Back3]), print_cube_row([' '|Left3]), nl,
    print_cube_row(Down1),nl,
    print_cube_row(Down2),nl,
    print_cube_row(Down3),nl.



% //////////////////////////////////////////////////////////////////////////////////////////////////// %
% ///////////////////////// Definition of 18 possible Ribik's cube rotations ///////////////////////// %
% //////////////////////////////////////////////////////////////////////////////////////////////////// %





% U/U' - Rotation of the top side of the cube 90° clockwise and counterclockwise
rotate_cube(up_ccw, Cube, RotatedCube) :- rotate_cube(up_cw, RotatedCube, Cube).
rotate_cube(up_cw,
    [
        [Up11, Up12, Up13], [Up21, Up22, Up23], [Up31, Up32, Up33],
        Front1, Front2, Front3,
        Right1, Right2, Right3,
        Back1, Back2, Back3,
        Left1, Left2, Left3,
        Down1, Down2, Down3
    ],
    [
        [Up31, Up21, Up11], [Up32, Up22, Up12], [Up33, Up23, Up13],
        Right1, Front2, Front3,
        Back1, Right2, Right3,
        Left1, Back2, Back3,
        Front1, Left2, Left3,
        Down1, Down2, Down3
    ]
).

% D/D' - Rotation of the bottom side of the cube 90° clockwise and counterclockwise
rotate_cube(down_ccw, Cube, RotatedCube) :- rotate_cube(down_cw, RotatedCube, Cube).
rotate_cube(down_cw,
    [
        Up1, Up2, Up3,
        Front1, Front2, Front3,
        Right1, Right2, Right3,
        Back1, Back2, Back3,
        Left1, Left2, Left3,
        [Down11, Down12, Down13], [Down21, Down22, Down23], [Down31, Down32, Down33]
    ],
    [
        Up1, Up2, Up3,
        Front1, Front2, Left3,
        Right1, Right2, Front3,
        Back1, Back2, Right3,
        Left1, Left2, Back3,
        [Down31, Down21, Down11], [Down32, Down22, Down12], [Down33, Down23, Down13]
    ]
).

% R/R' - Rotation of the right side of the cube 90° clockwise and counterclockwise
rotate_cube(right_ccw, Cube, RotatedCube) :- rotate_cube(right_cw, RotatedCube, Cube).
rotate_cube(right_cw,
    [
        [Up11, Up12, Up13], [Up21, Up22, Up23], [Up31, Up32, Up33],
        [Front11, Front12, Front13], [Front21, Front22, Front23], [Front31, Front32, Front33],
        [Right11, Right12, Right13], [Right21, Right22, Right23], [Right31, Right32, Right33],
        [Back11, Back12, Back13], [Back21, Back22, Back23], [Back31, Back32, Back33],
        Left1, Left2, Left3,
        [Down11, Down12, Down13], [Down21, Down22, Down23], [Down31, Down32, Down33]
    ],
    [
        [Up11, Up12, Front13], [Up21, Up22, Front23], [Up31, Up32, Front33],
        [Front11, Front12, Down13], [Front21, Front22, Down23], [Front31, Front32, Down33],
        [Right31, Right21, Right11], [Right32, Right22, Right12], [Right33, Right23, Right13],
        [Up33, Back12, Back13], [Up23, Back22, Back23], [Up13, Back32, Back33],
        Left1, Left2, Left3,
        [Down11, Down12, Back31], [Down21, Down22, Back21], [Down31, Down32, Back11]
    ]
).

% L/L' - Rotation of the left side of the cube 90° clockwise and counterclockwise
rotate_cube(left_ccw, Cube, RotatedCube) :- rotate_cube(left_cw, RotatedCube, Cube).
rotate_cube(left_cw,
    [
        [Up11, Up12, Up13], [Up21, Up22, Up23], [Up31, Up32, Up33],
        [Front11, Front12, Front13], [Front21, Front22, Front23], [Front31, Front32, Front33],
        Right1, Right2, Right3,
        [Back11, Back12, Back13], [Back21, Back22, Back23], [Back31, Back32, Back33],
        [Left11, Left12, Left13], [Left21, Left22, Left23], [Left31, Left32, Left33],
        [Down11, Down12, Down13], [Down21, Down22, Down23], [Down31, Down32, Down33]
    ],
    [
        [Back33, Up12, Up13], [Back23, Up22, Up23], [Back13, Up32, Up33],
        [Up11, Front12, Front13], [Up21, Front22, Front23], [Up31, Front32, Front33],
        Right1, Right2, Right3,
        [Back11, Back12, Down31], [Back21, Back22, Down21], [Back31, Back32, Down11],
        [Left31, Left21, Left11], [Left32, Left22, Left12], [Left33, Left23, Left13],
        [Front11, Down12, Down13], [Front21, Down22, Down23], [Front31, Down32, Down33]
    ]
).

% F/F' - Rotation of the front side of the cube 90° clockwise and counterclockwise
rotate_cube(front_ccw, Cube, RotatedCube) :- rotate_cube(front_cw, RotatedCube, Cube).
rotate_cube(front_cw,
    [
        Up1, Up2, [Up31, Up32, Up33],
        [Front11, Front12, Front13], [Front21, Front22, Front23], [Front31, Front32, Front33],
        [Right11, Right12, Right13], [Right21, Right22, Right23], [Right31, Right32, Right33],
        Back1, Back2, Right3,
        [Left11, Left12, Left13], [Left21, Left22, Left23], [Left31, Left32, Left33],
        [Down11, Down12, Down13], Down2, Down3
    ],
    [
        Up1, Up2, [Left33, Left23, Left13],
        [Front31, Front21, Front11], [Front32, Front22, Front12], [Front33, Front23, Front13],
        [Up31, Right12, Right13], [Up32, Right22, Right23], [Up33, Right32, Right33],
        Back1, Back2, Right3,
        [Left11, Left12, Down11], [Left21, Left22, Down12], [Left31, Left32, Down13],
        [Right31, Right21, Right11], Down2, Down3
    ]
).

% B/B' - Rotation of the back side of the cube 90° clockwise and counterclockwise
rotate_cube(back_ccw, Cube, RotatedCube) :- rotate_cube(back_cw, RotatedCube, Cube).
rotate_cube(back_cw,
    [
        [Up11, Up12, Up13], Up2, Up3,
        Front1, Front2, Front3,
        [Right11, Right12, Right13], [Right21, Right22, Right23], [Right31, Right32, Right33],
        [Back11, Back12, Back13], [Back21, Back22, Back23], [Back31, Back32, Back33],
        [Left11, Left12, Left13], [Left21, Left22, Left23], [Left31, Left32, Left33],
        Down1, Down2, [Down31, Down32, Down33]
    ],
    [
        [Right13, Right23, Right33], Up2, Up3,
        Front1, Front2, Front3,
        [Right11, Right12, Down33], [Right21, Right22, Down32], [Right31, Right32, Down31],
        [Back31, Back21, Back11], [Back32, Back22, Back12], [Back33, Back23, Back13],
        [Up13, Left12, Left13], [Up12, Left22, Left23], [Up11, Left32, Left33],
        Down1, Down2, [Left11, Left21, Left31]
    ]
).

% M/M' - Rotation of the middle vertical slice, parallel to the left and right sides, 90° clockwise and counterclockwise
rotate_cube(middle_ccw, Cube, RotatedCube) :- rotate_cube(middle_cw, RotatedCube, Cube).
rotate_cube(middle_cw,
    [
        [Up11, Up12, Up13], [Up21, Up22, Up23], [Up31, Up32, Up33],
        [Front11, Front12, Front13], [Front21, Front22, Front23], [Front31, Front32, Front33],
        Right1, Right2, Right3,
        [Back11, Back12, Back13], [Back21, Back22, Back23], [Back31, Back32, Back33],
        Left1, Left2, Left3,
        [Down11, Down12, Down13], [Down21, Down22, Down23], [Down31, Down32, Down33]
    ],
    [
        [Up11, Back32, Up13], [Up21, Back22, Up23], [Up31, Back12, Up33],
        [Front11, Up12, Front13], [Front21, Up22, Front23], [Front31, Up32, Front33],
        Right1, Right2, Right3,
        [Back11, Down32, Back13], [Back21, Down22, Back23], [Back31, Down12, Back33],
        Left1, Left2, Left3,
        [Down11, Front12, Down13], [Down21, Front22, Down23], [Down31, Front32, Down33]
    ]
).


% E/E' - Rotation of the equatorial horizontal slice 90° clockwise and counterclockwise
rotate_cube(equator_ccw, Cube, RotatedCube) :- rotate_cube(equator_cw, RotatedCube, Cube).
rotate_cube(equator_cw,
    [
        Up1, Up2, Up3,
        Front1, Front2, Front3,
        Right1, Right2, Right3,
        Back1, Back2, Back3,
        Left1, Left2, Left3,
        Down1, Down2, Down3
    ],
    [
        Up1, Up2, Up3,
        Front1, Left2, Front3,
        Right1, Front2, Right3,
        Back1, Right2, Back3,
        Left1, Back2, Left3,
        Down1, Down2, Down3
    ]
).

% S/S' - Rotation of the middle vertical slice, parallel to the front and back sides, 90° clockwise and counterclockwise
rotate_cube(slice_ccw, Cube, RotatedCube) :- rotate_cube(slice_cw, RotatedCube, Cube).
rotate_cube(slice_cw,
    [
        Up1, [Up21, Up22, Up23], Up3,
        Front1, Front2, Front3,
        [Right11, Right12, Right13], [Right21, Right22, Right23], [Right31, Right32, Right33],
        Back1, Back2, Back3,
        [Left11, Left12, Left13], [Left21, Left22, Left23], [Left31, Left32, Left33],
        Down1, [Down21, Down22, Down23], Down3
    ],
    [
        Up1, [Left32, Left22, Left12], Up3,
        Front1, Front2, Front3,
        [Right11, Up21, Right13], [Right21, Up22, Right23], [Right31, Up23, Right33],
        Back1, Back2, Back3,
        [Left11, Down21, Left13], [Left21, Down22, Left23], [Left31, Down23, Left33],
        Down1, [Right32, Right22, Right12], Down3
    ]
).


% //////////////////////////////////////////////////////////////////////////////////////////////////// %
% /////////////////////////////////////////// MAIN FUNCTION ////////////////////////////////////////// %
% //////////////////////////////////////////////////////////////////////////////////////////////////// %

% True if cube is solved else False
is_cube_solved(
    [
        [Up, Up, Up], [Up, Up, Up], [Up, Up, Up],
        [Front, Front, Front], [Front, Front, Front], [Front, Front, Front],
        [Right, Right, Right], [Right, Right, Right], [Right, Right, Right],
        [Back, Back, Back], [Back, Back, Back], [Back, Back, Back],
        [Left, Left, Left], [Left, Left, Left], [Left, Left, Left],
        [Down, Down, Down], [Down, Down, Down], [Down, Down, Down]
    ]
).


% Find solution of the given Cube
solve_cube(Cube, []) :- is_cube_solved(Cube).           % Check if cube is solved
solve_cube(Cube, [Rotation|RotationsHistory]) :-        % Try all rotations from level 0 (Initial cube)
    solve_cube(RotatedCube, RotationsHistory),          % Recursively continue to the next level
    rotate_cube(Rotation, Cube, RotatedCube).


% Print states of individual cube rotations
print_cube_rotation_history(_, []).
print_cube_rotation_history(Cube, [Rotation|RotationsHistory]) :- rotate_cube(Rotation, Cube, RotatedCube), nl, print_cube(RotatedCube), 
    print_cube_rotation_history(RotatedCube, RotationsHistory).


start :-
    prompt(_, ''),
    read_lines(LL),
    format_cube(LL, InputCube),
    solve_cube(InputCube, RotationsHistory),
    print_cube(InputCube),
    print_cube_rotation_history(InputCube, RotationsHistory),
    %nl, write(RotationsHistory), nl, % Prints the list of rotations leading to solution
    halt.


% /////////////////////////////////////////// END OF FILE //////////////////////////////////////////// %