/* BALAN Giorgiana-Lavinia - 311 CB */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "info.h"
#define DELIM " \n"


TLista AlocCel(void* x)	// functie de alocare celula
{
	//alocare spatiu celula si verificare
	TLista aux = (TLista) malloc(sizeof(Celula));
	if(!aux)
		return NULL;

	aux->info = x;
	aux->urm = NULL;

	return aux;
}


TLista AlocJuc(char* nume_juc, int nr_maini)	//alocare structura jucator
{
	//aloc spatiu pentru structura si verific alocarea
	Jucator j = (Jucator) malloc(sizeof(struct jucator));	
	if(!j)
		return NULL;

	//atribuire campuri pentru structura alocata
	j->nume = nume_juc;
	j->nrMaini = nr_maini;

	return AlocCel(j);
}


TLista AlocMasa(char* nume_masa, TLista ListaJuc, int nr_juc, int nr_max_juc)
{
	//aloc structura
	Masa m = (Masa) malloc(sizeof(struct masa));	
	if(!m)
		return NULL;

	//atribuire campuri pentru noua structura alocata
	m->numeMasa = nume_masa;
	m->jucatori = ListaJuc;
	m->nrCrtJucatori = nr_juc;
	m->nrMaxJucatori = nr_max_juc;

	return AlocCel(m);
}


TLista InitLCS()	// initializare lista circulara cu santinela: aloc santinela
{
	TLista s = AlocCel(NULL);
	if(!s)
		return NULL;

	s->urm = s;

	return s;
}


void LeagaJuc(TLista l, TLista juc_crt)	//legare juc la sf listei l
{
	TLista santi = l;

	//parcurg lista pana la sfarsit
	while(l->urm != santi)	
		l = l->urm;

	//fac legaturile
	l->urm = juc_crt;
	juc_crt->urm = santi;
}


void LeagaMasa(TLista *l, TLista masa_crt)	//legarea unei mese in sala
{
	if(!(*l))	//daca sala e goala
		*l = masa_crt;

	else	//daca mai sunt mese
	{
		//parcurg lista de mese pana ajung la ultima
		while((*l)->urm != NULL)
			l = &((*l)->urm);

		//leg masa la sfarsitul listei
		(*l)->urm = masa_crt;
	}
}


void muta_santi(TLista santinela)	//mutare santinela
{
	TLista first = santinela->urm;
	TLista last = santinela->urm;

	//caut ultimul elem din lista
	while(last->urm != santinela)
	{
		last = last->urm;
	}

	//refac legaturile
	last->urm = first;
	santinela->urm = first->urm;
	first->urm = santinela;
	
}


void verifElimJuc(Sala sala, Masa masa_crt)	//fct de elim a juc cu nr maini < 0
{
	TLista santinela = masa_crt->jucatori;
	TLista juc = santinela->urm;
	TLista ant_juc = santinela;

	//parcurg lista de juc
	while(juc != santinela)
	{
		Jucator juc_crt = (Jucator)(juc->info);

		if(juc_crt->nrMaini <= 0)
		{
			//scad numarul de juc de la masa dar si din sala
			(masa_crt->nrCrtJucatori)--;
			(sala->nrLocCrt)--;

			//elimin jucatorul
			ant_juc->urm = juc->urm;

			//eliberez memoria ocupata de juc
			free(juc_crt->nume);
			free(juc_crt);
			free(juc);

			juc = santinela;
		}

		//actualizez anteriorul si jucatorul curent
		ant_juc = juc;
		juc = juc->urm;
	}
}


void verifElimMasa(Sala sala)	//fct de elim masa vida
{
	TLista mese = sala->masa;
	TLista ant_masa = NULL;

	//parcurg lista de mese
	while(mese)
	{
		Masa masa_crt = (Masa)(mese->info);

		//verific daca mai sunt juc la masa
		if(masa_crt->nrCrtJucatori == 0)
		{	
			//verific daca e prima masa din lista 
			if(ant_masa == NULL)
				sala->masa = mese->urm;
			else
				ant_masa->urm = mese->urm;

			free(mese);

			//scad numarul de mese din sala
			(sala->nrMese)--;
		}

		//actualizez anteriorul si masa curenta
		ant_masa = mese;
		mese = mese->urm;
	}
}


void verifSala(Sala sala)	//verifica daca sala e goala
{
	if(sala->nrMese == 0)
		free(sala);
}


void scadeMaini(TLista santinela)	//fct pentru tura
{
	TLista juc = santinela->urm;

	//parcurg lista de juc si scad nr de maini pt fiecare juc
	while(juc != santinela)
	{
		Jucator juc_crt = (Jucator)(juc->info);

		(juc_crt->nrMaini)--;

		juc = juc->urm;
	}
}


void AfisareClas(FILE* out, TLista nou)	//fct pentru clasament
{
	TLista lista = nou->urm;
	Jucator info_crt;

	//parcurg lista
	while(lista != nou)
	{
		info_crt = (Jucator)(lista->info);

		fprintf(out, "%s %d\n", info_crt->nume, info_crt->nrMaini);
		
		lista = lista->urm;
	}
}


void InserareOrdonata(TLista jucnou, TLista listanoua)	//fct pentru clasament
{
	Jucator jucnouinfo = (Jucator)(jucnou->info);
	TLista Listadejuc = listanoua->urm;
	TLista antjuc = listanoua;

	//parcurg lista in care trebuie sa inserez
	while(Listadejuc != listanoua)
	{
		Jucator juc_crt_info = (Jucator)(Listadejuc->info);

		//daca gasesc un loc unde ar trebui inserat ma opresc
		if(jucnouinfo->nrMaini > juc_crt_info->nrMaini)
		{
			break;
		}
		
		//verific daca juc ce trebuie inserat si cel gasit au acelasi nr de maini
		if(jucnouinfo->nrMaini == juc_crt_info->nrMaini)
		{
			//compar dupa nume
			if(strcmp(jucnouinfo->nume, juc_crt_info->nume) > 0)
			{
				//fac legaturile, ma opresc
				jucnou->urm = Listadejuc;
				antjuc->urm = jucnou;
				return;
			}
		}

		//actualizare anterior si curent
		antjuc = Listadejuc;
		Listadejuc = Listadejuc->urm;
	
	}

	//refacere legaturi
	jucnou->urm = antjuc->urm;
	antjuc->urm = jucnou;
}


void DistrugeSala(Sala sala)
{
  TLista Lmese = sala->masa;
  while(Lmese)
  { //Elibereaza memoria alocata pentru mese
    Masa m = ((Masa)(Lmese->info));
    TLista santi = m->jucatori;
    TLista Ljuc = santi->urm;

    while(Ljuc != santi)
    { //Elibereaza memoria alocata pentru jucatori
      Jucator j = ((Jucator)(Ljuc->info));
      free(j->nume);
      free(j);
      TLista auxJ = Ljuc;
      Ljuc = Ljuc->urm;
      free(auxJ);
    }

    free(santi);
    free(m->numeMasa);
    TLista auxM = Lmese;
    Lmese = Lmese->urm;
    free(auxM);
 	}
  free(sala);
}


void print(FILE* out, Sala sala)
{
	TLista m = NULL;

	for(m = sala->masa; m; m = m->urm)	//parcurg lista de mese
	{
		//retin numele fiecarei mese si il afisez in out
		char* nume_masa = ((Masa)(m->info))->numeMasa;
		fprintf(out, "%s: ", nume_masa);

		TLista santi = ((Masa)(m->info))->jucatori;
		TLista j;
		
		for(j = santi->urm; j != santi; j = j->urm)	//parcurg lista de jucatori
		{
			//retin numele fiecarui jucator si nr de maini si le afisez in out
			char* nume_juc = ((Jucator)(j->info))->nume;
			int nr_maini = ((Jucator)(j->info))->nrMaini;
			fprintf(out, "%s - %d", nume_juc, nr_maini);

			if(j->urm == santi)
			{
				fprintf(out, ".\n");
			}
			else
			{
				fprintf(out, "; ");	
			}
		}
	}
}


void noroc(char n_masa[30], char n_juc[30], int grad, FILE* out, Sala sala)
{
	TLista mese = sala->masa;
	TLista juc = NULL, santinela = NULL;

	while(mese)
	{
		Masa masa_crt = ((Masa)(mese->info));

		//verific daca am gasit masa si ma opresc, daca nu, avansez in lista 
		if(strcmp(n_masa, masa_crt->numeMasa) == 0)
		{
			santinela = masa_crt->jucatori;
			juc = santinela->urm;
			break;
		}
		else
			mese = mese->urm;
	}

	//verific daca nu am gasit masa in sala, afisez mesaj si ma opresc
	if(mese == NULL)
	{
		fprintf(out, "Masa %s nu exista!\n", n_masa);
		return;
	}

	Jucator juc_crt_info = NULL;

	//parcurg lista de juc
	while(juc != santinela)
	{
		//retin informatia jucatorului curent
		juc_crt_info = ((Jucator)(juc->info));

		//verific daca este jucatorul cautat si ma opresc
		if(strcmp(n_juc, juc_crt_info->nume) == 0)
		{
			juc_crt_info->nrMaini += grad;
			break;
		}
		else
			juc = juc->urm;
	}

	//verific daca nu am gasit jucatorul
	if(juc == santinela)
	{
		fprintf(out, "Jucatorul %s nu exista la masa %s!\n", n_juc, n_masa);
		return;
	}
}


void ghinion(FILE* out, Sala sala, char n_masa[30], char n_juc[30], int grad)
{
	TLista mese = sala->masa;
	TLista juc = NULL, santinela = NULL;
	Masa masa_crt = NULL;
	TLista ant_masa = NULL;

	//parcurg lista de mese
	while(mese)
	{
		masa_crt = ((Masa)(mese->info));

		//daca am gasit masa cautata ma opresc
		if(strcmp(n_masa, masa_crt->numeMasa) == 0)
		{
			santinela = masa_crt->jucatori;
			juc = santinela->urm;
			break;
		}

		else
		{
			ant_masa = mese;
			mese = mese->urm;
		}
	}

	//verif daca masa cautata exista in sala
	if(mese == NULL)
	{
		fprintf(out, "Masa %s nu exista!\n", n_masa);
		return;
	}

	Jucator juc_crt = NULL;

	//parcurg lista de juc
	while(juc != santinela)
	{
		juc_crt = ((Jucator)(juc->info));

		//verif daca am  gasit jucatorul cautat si ma opresc
		if(strcmp(n_juc, juc_crt->nume) == 0)
		{
			juc_crt->nrMaini -= grad;
			break;
		}

		else
			juc = juc->urm;
	}

	//verif daca am gasit juc cautat la masa
	if(juc == santinela)
	{
		fprintf(out, "Jucatorul %s nu exista la masa %s!\n", n_juc, n_masa);
		return;
	}

	verifElimJuc(sala, masa_crt);
	verifElimMasa(sala);
	verifSala(sala);
}


void tura(FILE* out, Sala sala, char nume_masa[30])
{
	TLista mese = sala->masa, ant_masa = NULL, santinela = NULL, juc = NULL;
	Masa masa_crt = NULL;

	//parcurg lista de mese
	while(mese)
	{
		masa_crt = ((Masa)(mese->info));

		//verif daca am gasit masa cautata si ma opresc
		if(strcmp(nume_masa, masa_crt->numeMasa) == 0)
		{
			santinela = masa_crt->jucatori;
			juc = santinela->urm;
			break;
		}

		else
		{
			ant_masa = mese;
			mese = mese->urm;
		}
	}

	//verific daca masa exista
	if(mese == NULL)
	{
		fprintf(out, "Masa %s nu exista!\n", nume_masa);
		return;
	}

	muta_santi(santinela);
	scadeMaini(santinela);
	verifElimJuc(sala, masa_crt);
	verifElimMasa(sala);
	verifSala(sala);
}


void tura_completa(FILE* out, Sala sala)
{
	TLista mese = sala->masa;
	Masa masa_crt = NULL;

	while(mese)
	{
		masa_crt = (Masa)(mese->info);

		tura(out, sala, masa_crt->numeMasa);

		mese = mese->urm;
	}

}


void clasament(FILE* out, Sala sala, char nMasa[30])
{
	TLista ListaMese = sala->masa;
	Masa masaInfo = NULL;
	TLista santinela = NULL;

	//parcurg lista de mese 
	while(ListaMese != NULL)
	{
		masaInfo = (Masa)(ListaMese->info);

		//verific daca am gasit masa cautata
		if(strcmp(nMasa, masaInfo->numeMasa) == 0)
		{
			santinela = masaInfo->jucatori;
			break;
		}

		//	Avans in lista de mese
		ListaMese = ListaMese->urm;
	}

	if(ListaMese == NULL)
	{
		fprintf(out, "Masa %s nu exista!\n", nMasa);
		return;
	}

	//	Initializare lista noua circulara pentru inserare ordonata
	TLista ListaNoua = InitLCS();
	if(!ListaNoua)
		return;

	TLista ListaJuc = santinela->urm;

	while(ListaJuc != santinela)
	{
		TLista CelulaNoua = AlocCel(ListaJuc->info);
		if(!CelulaNoua)
			return;

		InserareOrdonata(CelulaNoua,ListaNoua);

		//	Avans in lista de jucatori
		ListaJuc = ListaJuc->urm;
	}

	fprintf(out, "Clasament %s:\n", masaInfo->numeMasa);
	AfisareClas(out, ListaNoua);
}


void inchide(Sala sala, char* nume_masa, FILE* out)
{
	TLista masa = sala->masa;
	Masa masa_info = NULL;
	Masa l_mese_info = NULL;
	TLista aux, juc, ant, lista_mese, santi1 = NULL, jucatori = NULL, masa_ant = NULL;

	//parcurg lista de mese pana gasesc masa ce trebuie inchisa
	while(masa != NULL)
	{
		masa_info = (Masa)(masa->info);

		if(strcmp(masa_info->numeMasa, nume_masa) == 0)
			break;

		//avans in lista
		masa = masa->urm;
	}

	int locuri, nr_max, nr_crt;

	if(masa == NULL)
	{
		fprintf(out, "Masa %s nu exista!\n", nume_masa);
		return;
	}

	else
	{
		//calculez numarul de locuri libere din sala
		nr_max = sala->nrLocMax - ((Masa)(masa->info))->nrMaxJucatori;
		nr_crt = sala->nrLocCrt - ((Masa)(masa->info))->nrCrtJucatori;
		locuri =  nr_max - nr_crt;

		//verific daca jucatorii de la masa in cauza nu au loc in sala
		if(((Masa)(masa->info))->nrCrtJucatori > locuri)
		{
			fprintf(out, "Nu exista suficiente locuri in sala!\n");
		}
		else
		{
			lista_mese = sala->masa;

			//parcurg lista de mese
			for(;masa_info->nrCrtJucatori > 0 && lista_mese != NULL; lista_mese = lista_mese->urm)
			{
				l_mese_info = (Masa)(lista_mese->info);

				if(lista_mese != masa)
				{
					while(l_mese_info->nrCrtJucatori < l_mese_info->nrMaxJucatori && masa_info->nrCrtJucatori >0)
					{
						juc = (l_mese_info->jucatori)->urm;

						for(; juc != l_mese_info->jucatori; ant = juc, juc = juc->urm);

						//refac legaturile
						aux = (masa_info->jucatori)->urm;
						ant->urm = aux;
						(masa_info->jucatori)->urm = aux->urm;
						aux->urm = l_mese_info->jucatori;

						//actualizez numarul de jucatori de la mese
						l_mese_info->nrCrtJucatori++;
						masa_info->nrCrtJucatori--;

					}
				}
			}
			verifElimMasa(sala);
		}

	}

}



int main(int argc, char const *argv[])
{	
	int nr_loc_crt = 0;
	int nr_loc_max = 0;

	//deschid fisierele date ca argument
	FILE *configuratie = fopen(argv[1], "r");
	if(!configuratie)
		return 1;
	FILE *evenimente = fopen(argv[2], "r");
	if(!evenimente)
		return 1;
	FILE *out = fopen(argv[3], "w");
	if(!out)
		return 1;

	//Creare sala
	Sala salaJoc = (Sala) malloc(sizeof(struct sala));
	if(!salaJoc)
		return 1;
	
	//citesc nr mese
	size_t dim = 100;
	char* buff = NULL;

	//iau cate o linie din fisierul configuratie
	getline(&buff, &dim, configuratie); 
	int nr_mese = atoi(buff);

	//creare lista de mese
	TLista lista_mese = NULL;
	
	//citeste mesele
	int i, j;
	char* temp = NULL;

	for (i = 0; i < nr_mese; i++)
	{
		getline(&buff, &dim, configuratie);
		temp = strtok(buff, DELIM);
		char* nume_masa = (char*) malloc(1 + strlen(temp));

		if(!nume_masa)
			free(nume_masa);

		memcpy(nume_masa, temp, strlen(temp));

		int nr_juc = atoi(strtok(NULL, DELIM));
		nr_loc_crt += nr_juc;

		int nr_max_juc = atoi(strtok(NULL, DELIM));
		nr_loc_max += nr_max_juc;

		//initializare lista circulara jucatori
		TLista ListaJuc = InitLCS();
		if(!ListaJuc)
			return 1;

		//creez masa curenta
		TLista masa_crt = AlocMasa(nume_masa, ListaJuc, nr_juc, nr_max_juc);
		if(!masa_crt)
		{	
			free(nume_masa);
			DistrugeSala(salaJoc);
			return -1;
		}

		//leg masa in lista
		LeagaMasa(&lista_mese, masa_crt);

		//citesc jucatorii de la fiecare masa cu numarul de maini
		for(j = 0; j < nr_juc; j++)
		{
			getline(&buff, &dim, configuratie);
			temp = strtok(buff, DELIM);
			char* nume_juc = (char*) malloc(1 + strlen(temp));

			if(!nume_juc)
				free(nume_juc);

			memcpy(nume_juc, temp, strlen(temp));
			int nr_maini = atoi(strtok(NULL, DELIM));

			TLista juc_crt = AlocJuc(nume_juc, nr_maini);

			if(!juc_crt)
			{
				free(nume_juc);
				free(nume_masa);
				DistrugeSala(salaJoc);
				return -1;
			}

			//leaga jucator in lista
			LeagaJuc(ListaJuc, juc_crt);
		}

	}

	salaJoc->masa = lista_mese;
	salaJoc->nrMese = nr_mese;
	salaJoc->nrLocCrt = nr_loc_crt;
	salaJoc->nrLocMax = nr_loc_max;
	
	//apelare functii

	char event[30], masa[30], jucator[30];
	int grad_noroc, grad_ghinion;

	while(fscanf(evenimente, "%s", event) != EOF)
	{
		if(strcmp(event, "print") == 0)
		{
			print(out, salaJoc);
		}
		else
		{
			if(strcmp(event, "noroc") == 0)
			{
				fscanf(evenimente, "%s", masa);
				fscanf(evenimente, "%s", jucator);
				fscanf(evenimente, "%d", &grad_noroc);
				noroc(masa, jucator, grad_noroc, out, salaJoc);
			}
			else
			{
				if(strcmp(event, "ghinion") == 0)
				{
					fscanf(evenimente, "%s", masa);
					fscanf(evenimente, "%s", jucator);
					fscanf(evenimente, "%d", &grad_ghinion);
					ghinion(out, salaJoc, masa, jucator, grad_ghinion);
				}
				else
				{
					if(strcmp(event, "tura") == 0)
					{
						fscanf(evenimente, "%s", masa);
						tura(out, salaJoc, masa);
					}
					else
					{
						if(strcmp(event, "tura_completa") == 0)
							tura_completa(out, salaJoc);
						else
						{
							if(strcmp(event, "clasament") == 0)
							{
								fscanf(evenimente, "%s", masa);
								clasament(out, salaJoc, masa);
							}
							else
							{
								if(strcmp(event, "inchide") == 0)
								{
									fscanf(evenimente, "%s", masa);
									inchide(salaJoc, masa, out);
								}
							}
						}
					}
				}
			}
		}
	}

	DistrugeSala(salaJoc);

	//inchidere fisiere deschise
	fclose(configuratie);
	fclose(evenimente);
	fclose(out);
	return 0;
}
