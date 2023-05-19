
%
% Co je na indexe
%

atIndex([H|_], INDEX, VALUE) :- INDEX =:= 0, VALUE is H, !.
atIndex([_|T], INDEX, VALUE) :- NewINDEX is INDEX - 1, atIndex(T, NewINDEX, VALUE).


%
% Na ktorom indexe je cislo NUM
%

itemAt([H|_], ITEM, INDEX) :- H =:= ITEM, INDEX is 0.
itemAt([_|T], ITEM, INDEX) :- itemAt(T, ITEM, NewINDEX), INDEX is NewINDEX + 1.


%
% Najdi index najvecsieho prvku v zozname
%


% ak mame zoznam s jednym prvkom
maxval([NUM], NUM, 0) :- !.

% ak mame zoznam s viacerimi prvkami a prvy prvok je vesci ako MAX
maxval([H|T], MAX, NewINDEX) :- maxval(T, TailMAX, _), TailMAX < H, MAX is H, NewINDEX is 0.

% ak mame zoznam s viacerimi prvkami a prvy prvok je mensi ako MAX
maxval([H|T], MAX, NewINDEX) :- maxval(T, TailMAX, INDEX), TailMAX >= H, MAX is TailMAX, NewINDEX is INDEX + 1.


%
% mnozina zo zoznamu + isInList(LIST, ITEM)
%

% true. ak je prvok v zozname inac false.
isInList([H|_], ITEM) :- ITEM =:= H, !.
isInList([_|T], ITEM) :- isInList(T,ITEM).

% prevod zoznamu na mnozinu
list2set([], []).
list2set([H|T], SET) :- isInList(T, H), !, list2set(T, SET).
list2set([H|T], [H|SET]) :- list2set(T, SET).


%
% Zjednotenie dvoch množín
%

union(S1, [], S1).
union(S1, [H|T], R) :- isInList(S1,H), !, union(S1, T, R).
union(S1, [H|T], [H|R]) :- union(S1, T, R).


%
% Prienik dvoch množín
%

intersec(_,[],[]).
intersec(S1,[H|T],[H|R]) :- isInList(S1,H), !, intersec(S1,T,R).
intersec(S1,[_|T],R) :- intersec(S1,T,R).


%
% Medián zo zoradeného zoznamu s nepárnym počom prvkov
%

% dlzka zoznamu
len([], 0).
len([_|T], LEN) :- len(T, NewLEN), LEN is NewLEN + 1.

% median
median(LIST, VALUE) :- len(LIST, LEN), INDEX is LEN // 2, getMedian(LIST, INDEX, VALUE).

getMedian([H|_], INDEX, VALUE) :- INDEX =:= 0, VALUE is H, !.
getMedian([_|T], INDEX, VALUE) :- NewINDEX is INDEX - 1, getMedian(T, NewINDEX, VALUE).


%
% Otocenie zoznamu
%

myAppend([],ITEM,ITEM).
myAppend([H|T],ITEM,[H|NewLIST]) :- myAppend(T,ITEM,NewLIST).

reverseList([],[]).
reverseList([H|T],LIST) :- reverseList(T,NewLIST), myAppend(NewLIST,[H],LIST).

%
% Pridanie minima zo zoznamu na koniec zoznamu
%

% najdenie minima
myMin([H|T],MIN) :- myMin(T,H,MIN).
myMin([],MIN,MIN).
myMin([H|T],CurrMIN,MIN) :- H < CurrMIN -> myMin(T,H,MIN); myMin(T,CurrMIN,MIN).

insMin(L1,L2) :- myMin(L1,MIN), myAppend(L1,[MIN],L2).


%
% Zistenie či je list/zoznam množinou
%

isSet([]).
isSet([H|T]) :- \+ isInList(T,H), isSet(T).

%
% Vloženie prvku na zadaný index
%

insertAtIndex(ITEM,0,T,[ITEM|T]).
insertAtIndex(ITEM,INDEX,[H|T],[H|OUT]) :- INDEX > 0, NewINDEX is INDEX - 1, insertAtIndex(ITEM,NewINDEX,T,OUT).


%
% Zluc 2 usporiadane zoznamy
%

zluc([],T2,T2).
zluc(T1,[],T1).
zluc([H1|T1],[H2|T2],[H1|OUT]) :- H1 =< H2, zluc(T1,[H2|T2],OUT).
zluc([H1|T1],[H2|T2],[H1|OUT]) :- H1 > H2, zluc([H1|T1],T2,OUT).


%
% Nájdi maximum
%

myMax([H|T], MAX) :- myMax(T,H,MAX).
myMax([],MAX,MAX).
myMax([H|T],CurrMAX,MAX) :- H > CurrMAX -> myMax(T,H,MAX); myMax(T,CurrMAX,MAX).

myMax2([H],H).
myMax2([H|T],MAX) :- myMax(T,TMP), (H > TMP -> MAX is H; MAX is TMP).

%
% Vymaz so zoznamu zadany item
%

delItem([],_,[]).
delItem([H|T],ITEM,OUT) :- H =:= ITEM, delItem(T,ITEM,OUT).
delItem([H|T],ITEM,[H|OUT]) :- delItem(T,ITEM,OUT).