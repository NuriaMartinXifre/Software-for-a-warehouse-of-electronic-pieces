#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<fstream>
using namespace std;
const int N=600,M=121,plantes=3,passadissos=20,seccions=10,capacitat_max=100,min_generar=0,max_generar=9,num_generar=9,C=20,min_stock=5;
struct tUbicacio{
	int planta,passadis,seccio;
};
struct tTemps_mod{
	string tipus_mod;
	int dia,mes,any,hora,minuts,segons;	
};
struct tproductes{
	long long int codi;
	int unitats,uvenudes=0;
	double preu;
	tUbicacio ub;
	string nom,fabricant;
	tTemps_mod temps_mod;
};
struct tpais{
	string nom;
	int cod;
};

void temps(struct tm*info,tproductes producte[N],int index_p){
	time_t t;				
	time(&t);
	info=localtime(&t);		
	producte[index_p].temps_mod.dia=info->tm_mday;
	producte[index_p].temps_mod.mes=1+info->tm_mon;
	producte[index_p].temps_mod.any=1900+info->tm_year;
	producte[index_p].temps_mod.hora=info->tm_hour;
	producte[index_p].temps_mod.minuts=info->tm_min;
	producte[index_p].temps_mod.segons=info->tm_sec;
}

void imprimir_pantalla(tproductes producte[N],int index_p){
	cout<<"Producte: "<<producte[index_p].nom<<endl;
	cout<<"Codi: "<<producte[index_p].codi<<endl;
	cout<<"Fabricant: "<<producte[index_p].fabricant<<endl;
	cout<<"Preu: "<<producte[index_p].preu<<endl;
	cout<<"Unitats: "<<producte[index_p].unitats<<endl;	
	cout<<"Unitats venudes: "<<producte[index_p].uvenudes<<endl;
	cout<<"Planta: "<<producte[index_p].ub.planta<<"  Passadis: "<<producte[index_p].ub.passadis<<"  Seccio: "<<producte[index_p].ub.seccio<<endl;
	cout<<"Darrera modificacio al producte (DD/MM/AA  HH:MM:SS): "<<endl;
	cout<<producte[index_p].temps_mod.tipus_mod<<": "<<producte[index_p].temps_mod.dia<<"/"<<producte[index_p].temps_mod.mes<<"/"<<producte[index_p].temps_mod.any;
	cout<<"\t"<<producte[index_p].temps_mod.hora<<":"<<producte[index_p].temps_mod.minuts<<":"<<producte[index_p].temps_mod.segons<<endl;
	cout<<endl;
}

void determinar_posicio(tproductes producte[N],int i,tUbicacio espais_buits[N],int&nespais,tUbicacio&ultim_lliure){
	if(nespais!=0){
		producte[i].ub.planta=espais_buits[0].planta;
		producte[i].ub.passadis=espais_buits[0].passadis;
		producte[i].ub.seccio=espais_buits[0].seccio;
		for(int j=0;j<nespais-1;j++){
			espais_buits[j]=espais_buits[j+1];
		}
		nespais--;
	}
	else{
		producte[i].ub.planta=ultim_lliure.planta;
		producte[i].ub.passadis=ultim_lliure.passadis;
		producte[i].ub.seccio=ultim_lliure.seccio;
		if(ultim_lliure.seccio<seccions) ultim_lliure.seccio++;
		else{
			ultim_lliure.seccio=1;
			if(ultim_lliure.passadis<passadissos) ultim_lliure.passadis++;
			else{
				ultim_lliure.passadis=1;
				ultim_lliure.planta++;
			}
		}
	}
}

bool cerca_binaria(tproductes m[N],long long int x,int nelements,int&index){
	int mig,a=0,b=nelements-1;
	while(a<=b){
		mig=(b+a)/2;
		if(m[mig].codi<x){
			a=mig+1;
		}
		else if(m[mig].codi>x){
			b=mig-1;
		}
		else{
			index=mig;
			return true;
		}
	}
	return false;
}

void intercanviar(tproductes producte[N],int j,int k){
	tproductes aux;
	aux=producte[j];
	producte[j]=producte[k];
	producte[k]=aux;
}

void ordenar(tproductes producte[N],int nelements){
	for(int g=0;g<nelements-1;g++){
		for(int j=0;j<(nelements-g-1);j++){
			if(producte[j].codi>producte[j+1].codi) intercanviar(producte,j,j+1);
		}
	}
}

int fitxer_paisos(string nom){
	tpais p[M];
    string linia;
    ifstream fitxer_paisos("paisos_be.txt");
    int i=0;
    if(fitxer_paisos.is_open()){
    	while(!fitxer_paisos.eof()){
    		getline(fitxer_paisos,linia,';');
    		p[i].cod=atoi(linia.c_str()); 
    		getline(fitxer_paisos,linia,';');
    		p[i].nom=linia;
    		i++;
    	}
    	fitxer_paisos.close(); 
    	for(int j=0;j<i-1;j++){
    		if(p[j].nom==nom){
    			return p[j].cod;
			}
		}
	}
	return -1;
}

int	genera_aleatori(){					
	return rand()%(max_generar-min_generar+1)+min_generar;						
}		

int	EAN	(long long int num){					
	if(num<100){									
		return (num%10)*3+num/10;	
	}					
	else{									
		return (num%10)*3+(num%100)/10+EAN(num/100);					
	}						
}		

int	DC(long long int num){					
	if(EAN(num)%10==0) return 0;					
	else{									
		return (EAN(num)/10)*10+10-EAN(num);					
	}				
}
	
void donar_alta(tproductes producte[N],int&i,tUbicacio espais_buits[N],int&nespais,tUbicacio&ultim_lliure){
	if(i!=N){
		int unitatsn,opcio_codi_pais,codi_pais;
		long long int codi;
		string nom_pais;
		cout<<"1-Introduir nom del pais\n2-Codi del pais"<<endl;
		cin>>opcio_codi_pais;
		if(opcio_codi_pais==1){
			cin>>nom_pais;
			codi_pais=fitxer_paisos(nom_pais);
		}		
		else if(opcio_codi_pais==2){
			cout<<"Introdueix el codi del pais: ";
			cin>>codi_pais;
		}
		if(codi_pais>-1&&codi_pais<1000){	
			codi=codi_pais;
			srand(time(NULL));						
			for	(int g=1;g<=num_generar;g++){									
				codi=codi*10+genera_aleatori();				
			}				
			codi=codi*10+DC(codi);
			producte[i].codi=codi;
			cout<<"Introdueix les unitats: ";	
			cin>>unitatsn;
			if(unitatsn<capacitat_max){
				determinar_posicio(producte,i,espais_buits,nespais,ultim_lliure);
				producte[i].unitats=unitatsn;							
				cout<<"Introdueix el nom del producte: ";
				cin>>producte[i].nom;
				cout<<"Introdueix el nom del fabricant: ";
				cin>>producte[i].fabricant;
				cout<<"Introdueix el preu: ";
				cin>>producte[i].preu;
				producte[i].temps_mod.tipus_mod="Donar d'alta";
				struct tm*info_temps;
				temps(info_temps,producte,i);
				cout<<"El codi de barres es: "<<producte[i].codi<<endl;
				i++;
				ordenar(producte,i);
				cout<<"Producte afegit correctament."<<endl;
			}
			else{
				cout<<"La quantitat supera el limit establert."<<endl;	
			}
		}
		else{
			cout<<"El pais indicat no esta en la llista."<<endl;
		}
	}
	else{
		cout<<"El magatzem esta ple."<<endl;
	}
}

bool cercar(tproductes producte[N],int nelements,int&index){
	long long int codi;
	cout<<"Introdueix el codi del producte: ";
	cin>>codi;
	if(cerca_binaria(producte,codi,nelements,index)){
		imprimir_pantalla(producte,index);
		return true;
	}
	return false;
}

void donar_baixa(tproductes producte[N],int&nelements,tUbicacio espais_buits[N],int&nespais){
	int codi_eliminar,index_eliminar,index;
	if(cercar(producte,nelements,index)){
		espais_buits[nespais].planta=producte[index].ub.planta;
		espais_buits[nespais].passadis=producte[index].ub.passadis;
		espais_buits[nespais].seccio=producte[index].ub.seccio;
		nespais++;
		for(int j=index;j<nelements-1;j++){
			producte[j]=producte[j+1];
		}
		nelements--;
		cout<<"Producte eliminat."<<endl;
	}
	else{
		cout<<"Producte no trobat."<<endl;
	}
}

bool trobat(int planta,int passadis,int seccio,tUbicacio espais_buits[N],int nespais,int&index){
	for(int i=0;i<nespais;i++){
		if(planta==espais_buits[i].planta&&passadis==espais_buits[i].passadis&&seccio==espais_buits[i].seccio){
			index=i;
			return true;
		}
	}
	return false;
}

void modificar_ubicacio(tproductes producte[N],int nelements,int i,tUbicacio espais_buits[N],int&nespais,tUbicacio&ultim_lliure){
	cout<<"Aquests son els llocs on es pot assignar el producte:"<<endl;
	if(ultim_lliure.planta!=0){
	cout<<"Planta: "<<ultim_lliure.planta<<" Passadis: "<<ultim_lliure.passadis<<" Seccio: "<<ultim_lliure.seccio<<endl;
	}
	for(int k=0;k<nespais;k++){
		cout<<"Planta: "<<espais_buits[k].planta<<" Passadis: "<<espais_buits[k].passadis<<" Seccio: "<<espais_buits[k].seccio<<endl;
	}
	cout<<"Introdueix la planta, el passadis i la seccio: ";
	int planta,passadis,seccio,index;
	cin>>planta>>passadis>>seccio;
	if(planta==ultim_lliure.planta&&passadis==ultim_lliure.passadis&&seccio==ultim_lliure.seccio&&ultim_lliure.planta<=plantes){
		espais_buits[nespais].planta=producte[i].ub.planta;
		espais_buits[nespais].passadis=producte[i].ub.passadis;
		espais_buits[nespais].seccio=producte[i].ub.seccio;
		nespais++;
		producte[i].ub.planta=planta;
		producte[i].ub.passadis=passadis;
		producte[i].ub.seccio=seccio;
		if(ultim_lliure.seccio<seccions) ultim_lliure.seccio++;
		else{
			ultim_lliure.seccio=1;
			if(ultim_lliure.passadis<passadissos) ultim_lliure.passadis++;
			else{
				ultim_lliure.passadis=1;
				ultim_lliure.planta++;
			}
		}
	}
	else if(trobat(planta,passadis,seccio,espais_buits,nespais,index)){
		for(int j=index;j<nespais;j++){
			espais_buits[j]=espais_buits[j+1];
		}
		nespais--;
		producte[i].ub.planta=planta;
		producte[i].ub.passadis=passadis;
		producte[i].ub.seccio=seccio;
	}
	else{
		cout<<"La ubicacio no correspon a cap de les disponibles."<<endl;
	}
}

void modificar(tproductes producte[N],int nelements,tUbicacio espais_buits[N],int&nespais,tUbicacio&ultim_lliure){
	int index,menu_modificar=0;
	if(cercar(producte,nelements,index)){
		while(menu_modificar!=5){
		cout<<"\nQuin camp vol modificar?\n1-Nom\n2-Fabricant\n3-Preu\n4-Ubicacio\n5-Sortir"<<endl;
		cin>>menu_modificar;
		switch(menu_modificar)
		{
			case 1:
				cout<<"Introdueix el nou nom: ";
				cin>>producte[index].nom;
				break;
			case 2:
			    cout<<"Introdueix el nou fabricant: ";
				cin>>producte[index].fabricant;
				break;		        
			case 3:
			    cout<<"Introdueix el nou preu: ";
				cin>>producte[index].preu;
				break;
			case 4:
				modificar_ubicacio(producte,nelements,index,espais_buits,nespais,ultim_lliure);
				break;
			default:
				cout<<"Operacio no valida."<<endl;
				break;
		}
		}
		producte[index].temps_mod.tipus_mod="Modificacio";
		struct tm*info_temps;
		temps(info_temps,producte,index);
	}
	else{
		cout<<"No s'ha trobat aquest producte."<<endl;
	}
}

void cercar_mostrar(tproductes producte[N],int nelements){
	string criteri,nom,fab,nomprod;
	int codi,i=0,index;
	cout<<"Introdueix el criteri de busqueda (nom, codi o fabricant)"<<endl;
	cin>>criteri;
	if(criteri=="nom"){
		cout<<"Introdueix el nom del producte: ";
		cin>>nomprod;
		int productes_nomprod[N],p_nomprod=0;
		for(int k=0;k<nelements;k++){
			if(nomprod==producte[k].nom){
				productes_nomprod[p_nomprod]=k;
				p_nomprod++;
			}                    
		}
		if(p_nomprod==0){
			cout<<"No s'ha trobat cap producte amb el nom indicat."<<endl;
		}
		else{
			for(int compt_n=0;compt_n<p_nomprod;compt_n++){
				imprimir_pantalla(producte,productes_nomprod[compt_n]);
			}
		}
	}
	else if(criteri=="codi"){
		cout<<"Introdueix el codi del producte: ";
		cin>>codi;
		if(cerca_binaria(producte,codi,nelements,index)){
			imprimir_pantalla(producte,index);
		}
		else{
			cout<<"Producte no trobat."<<endl;
		}
	}
	else if(criteri=="fabricant"){
		cout<<"Introdueix el fabricant del producte"<<endl;
		cin>>fab;
		int productes_fabricants[N],p_fabricants=0;
		for(int k=0;k<nelements;k++){
			if(fab==producte[k].fabricant){
				productes_fabricants[p_fabricants]=k;
				p_fabricants++;
			}                    
		}
		if(p_fabricants==0){
			cout<<"No s'ha trobat cap producte fet pel fabricant indicat."<<endl;
		}
		else{
			for(int compt_f=0;compt_f<p_fabricants;compt_f++){
				imprimir_pantalla(producte,productes_fabricants[compt_f]);
			}
		}
	}
	else{
		cout<<"Criteri incorrecte"<<endl;
	}
}

void minim_stock(tproductes producte[N],int nelements){
	for(int i=0;i<nelements;i++){
		if(producte[i].unitats<min_stock){
			imprimir_pantalla(producte,i);
		}
	}
}

void venda(tproductes producte [N], int nelements){
	long long int codi;
	int k;
	int unitatsv;
	cout<<"Introdueix el codi del producte a vendre: "<<endl; 
	cin>>codi;
	if(cerca_binaria(producte,codi,nelements,k)){
	if(producte[k].unitats!=0){
		cout<<"Quantes unitats es venen?"<<endl;
		cin>>unitatsv;
		if((producte[k].unitats-unitatsv)>=0){
			producte[k].unitats-=unitatsv;
			producte[k].uvenudes+=unitatsv;
			cout<<"Venda realitzada correctament."<<endl;
		}
		else{
			cout<<"No hi ha suficient stock per a realitzar aquesta venda."<<endl;
		}
		}
	else{
		cout<<"No hi ha unitats d'aquest producte."<<endl;
	}
	}
	else{
		cout<<"No s'ha trobat el producte amb el codi especificat."<<endl;
	}
}

void comanda(tproductes producte[N],int nelements){
	long long int codicomanda;
	int ucomanda,index;
	cout<<"Aquests son els productes a punt d'esgotar-se:\n\n";
	minim_stock(producte,nelements);
	cout<<"Introdueix el codi: "<<endl;
	cin>>codicomanda;
	if(cerca_binaria(producte,codicomanda,nelements,index)){
		cout<<"Quantes unitats es volen comprar? "; 
		cin>>ucomanda;
		if(ucomanda+producte[index].unitats>capacitat_max){
			cout<<"Es sobrepassa la capacitat maxima de la seccio."<<endl;
		}
		else{
			producte[index].unitats+=ucomanda;
			cout<<"Comanda realitzada correctament."<<endl;
		}
	}
	else{
		cout<<"o s'ha trobat el producte amb el codi especificat."<<endl;
	}
}

void ordenar_vendes(tproductes vendes[N],int nelements){
	for (int i=0;i<nelements-1;i++){
		for (int j=0;j<nelements-i-1;j++){
			if(vendes[j].uvenudes>vendes[j+1].uvenudes){
				intercanviar(vendes,j,j+1);
			}
		}
		imprimir_pantalla(vendes,nelements-i-1);
	}
	imprimir_pantalla(vendes,0);
}

void llista_vendes(tproductes producte[N],int nelements){
	tproductes vendes[N];
	for(int i=0;i<nelements;i++){
		vendes[i]=producte[i];
	}
	ordenar_vendes(vendes,nelements);
}

void recuperar_dades(tproductes producte [N],int&n){
	string linia;
	ifstream fitxer_productes("datos.txt");
	int i=0; 
	if(fitxer_productes.is_open()){
    	while(!fitxer_productes.eof()){
    		getline(fitxer_productes,linia,';');
    		producte[i].nom=linia;
    		getline(fitxer_productes,linia,';');
    		producte[i].codi=atof(linia.c_str()); 
    		getline(fitxer_productes,linia,';');
    		producte[i].fabricant=linia;
			getline(fitxer_productes,linia,';');
			producte[i].preu=atoi(linia.c_str());
			getline(fitxer_productes,linia,';');
			producte[i].unitats=atoi(linia.c_str());
			getline(fitxer_productes,linia,';');
			producte[i].uvenudes=atoi(linia.c_str());
			getline(fitxer_productes,linia,';');
			producte[i].ub.planta=atoi(linia.c_str());
			getline(fitxer_productes,linia,';');
			producte[i].ub.passadis=atoi(linia.c_str());
			getline(fitxer_productes,linia,';');
			producte[i].ub.seccio=atoi(linia.c_str());
			getline(fitxer_productes,linia,';');
			producte[i].temps_mod.dia=atoi(linia.c_str());
			getline(fitxer_productes,linia,';');
			producte[i].temps_mod.mes=atoi(linia.c_str());
			getline(fitxer_productes,linia,';');
			producte[i].temps_mod.any=atoi(linia.c_str());
			getline(fitxer_productes,linia,';');
			producte[i].temps_mod.hora=atoi(linia.c_str());
			getline(fitxer_productes,linia,';');
			producte[i].temps_mod.minuts=atoi(linia.c_str());
			getline(fitxer_productes,linia,';');
			producte[i].temps_mod.segons=atoi(linia.c_str());
			i++;
		}	
		n=i-1; 
		fitxer_productes.close(); 
		for(int j=0;j<n;j++){
 			imprimir_pantalla(producte,j);
		}
	}
	else{
		cout<<"No s'ha pogut obrir el fitxer."<<endl;
	}
}

void recuperar_espais(tUbicacio espais_buits[N], int&nespais){
	string linia;
	ifstream fitxer("espais.txt");
	int i=0; 
	if(fitxer.is_open()){
    	while(!fitxer.eof()){
    		getline(fitxer,linia,';');
    		espais_buits[i].planta=atoi(linia.c_str());
    		getline(fitxer,linia,';');
    		espais_buits[i].passadis=atoi(linia.c_str());;
    		getline(fitxer,linia,';');
    		espais_buits[i].seccio=atoi(linia.c_str());;
    		i++;
		}	
		nespais=i-1; 
		fitxer.close(); 
	}
}

void guardar_dades(tproductes producte[N],int&n){
	ofstream fitxer_productes("datos.txt");	
	for(int i=0;i<n;i++){
		fitxer_productes << producte[i].nom<<';';
		fitxer_productes << producte[i].codi<<';';
		fitxer_productes << producte[i].fabricant<<';';
		fitxer_productes << producte[i].preu<<';';
		fitxer_productes << producte[i].unitats<<';';
		fitxer_productes << producte[i].uvenudes<<';';
		fitxer_productes << producte[i].ub.planta<<';';	
		fitxer_productes << producte[i].ub.passadis<<';';
		fitxer_productes << producte[i].ub.seccio<<';';
		fitxer_productes << producte[i].temps_mod.dia<<';';
		fitxer_productes << producte[i].temps_mod.mes<<';';
		fitxer_productes << producte[i].temps_mod.any<<';';
		fitxer_productes << producte[i].temps_mod.hora<<';';
		fitxer_productes << producte[i].temps_mod.minuts<<';';
		fitxer_productes << producte[i].temps_mod.segons<<';';
	}
	fitxer_productes.close();
}

void guardar_espais(tUbicacio espais_buits[N],int&nespais){
	ofstream fitxer("espais.txt");	
	for(int i=0;i<nespais;i++){
		fitxer << espais_buits[i].planta<<';';
		fitxer << espais_buits[i].passadis<<';';
		fitxer << espais_buits[i].seccio<<';';
	}
	fitxer.close();
}


int main(void){
	tproductes producte[N];
	tUbicacio espais_buits[N],ultim_lliure; ultim_lliure.planta=1; ultim_lliure.passadis=1; ultim_lliure.seccio=1;
	int menu=0,nelements=0,nespais=0;
	while(menu!=6){
		system("cls");
		cout<<"Magatzem de Peces Electroniques"<<endl;
		cout<<"Menu principal"<<endl;
		cout<<"1-Gestionar productes\n2-Cercar productes\n3-Control de stocks\n4-Recuperar dades\n5-Guardar dadades\n6-Sortir del programa"<<endl;
		cin>>menu;
		switch(menu){
			case 1:
				int ges;
				while(ges!=4){
				system("cls");
				cout<<"Menu gestionar productes"<<endl;
				cout<<"1-Donar d'alta un producte\n2-Donar de baixa un producte\n3-Modificar un producte\n4-Tornar al menu principal"<<endl;
				cin>>ges;
				switch(ges){
					case 1:
						donar_alta(producte,nelements,espais_buits,nespais,ultim_lliure);
						break;
					case 2:
						donar_baixa(producte,nelements,espais_buits,nespais);
						break;
					case 3:
						modificar(producte,nelements,espais_buits,nespais,ultim_lliure);
						break;
					case 4:
						cout<<"Ha tancat el menu de gestio d'estocs."<<endl;
						break;
					default:
						cout<<"Operacio no valida."<<endl;
						break;
				}
				}
				ges=0;
				break;
			case 2:
				system("cls");
				cercar_mostrar(producte,nelements);
				break;
			case 3:
				int opcstock;
				while(opcstock!=5){
				system("cls");
				cout<<"Menu stock:"<<endl;
				cout<<"1-Llistar productes inferiors al minim de stock\n2-Venda de productes\n3-Realitzar comanda\n4-Llistar productes segons les vendes\n5-Tornar al menu principal"<<endl;
				cin>>opcstock;
				switch(opcstock){
					case 1:
						minim_stock(producte,nelements);
						break;
					case 2:
						venda(producte,nelements);
						break;
					case 3:
						comanda(producte,nelements);
						break;
					case 4:
						llista_vendes(producte,nelements);
						break;
					case 5:
						cout<<"Ha tancat el menu de control d'estocs."<<endl;
						break;
					default:
						cout<<"Operacio no valida."<<endl;
						break;
				}
				}
				opcstock=0;
				break;
			case 4:
				recuperar_dades(producte,nelements);
				recuperar_espais(espais_buits,nespais);
				break;
			case 5:
				guardar_dades(producte,nelements);
				guardar_espais(espais_buits,nespais);
				break;
			case 6:
				cout<<"Ha tancat el programa."<<endl;
				break;
			default:
				cout<<"Operacio no valida."<<endl;
				break;
		}
	}
	system("pause");	
}
