% Zadani c. 30:
% Napiste program resici ukol dany predikatem u30(LIN,VOUT), kde LIN je  
% vstupni ciselny seznam s nejmene jednim prvkem a VOUT je promenna, ve 
% ktere se vraci prumerna hodnota absolutnich hodnot cisel v seznamu LIN. 

% Testovaci predikaty:                                 		% VOUT 
u30_1:- u30([5,3,-18,2,-9,-13,17],VOUT),write(VOUT).		% 9.57...
u30_2:- u30([5,2],VOUT),write(VOUT).				% 3.5
u30_3:- u30([-18.3,-9.2,-13.1],VOUT),write(VOUT).		% 13.53...
u30_r:- write('Zadej LIN: '),read(LIN),
	u30(LIN,VOUT),write(VOUT).

% Reseni:
u30(LIN,VOUT):- my_sum_len(LIN,S,L), L =\= 0 -> VOUT is S/L ; VOUT is 0.

my_sum_len([],0,0).
my_sum_len([HEAD|TAIL],SUM,LEN):- my_sum_len(TAIL,SUM1,LEN1), my_abs(HEAD, ABS_HEAD), SUM is SUM1+ABS_HEAD, LEN is LEN1+1.

my_abs(NUM,RES):- NUM < 0 -> RES is -NUM ; RES = NUM.