/*  INF3173 - TP0
 *  Session : automne 2021
 *  Tous les groupes
 */

 /*
  * Oriol Camps Pérez
  * CAMO93010104
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

//zone de déclaration des fonctions
int separer_OFFSET_BYTES(char *param, int *OFFSET,int *BYTES);
int file_size(int fd);
void isoler_nom_fichier(char *path, char **nom_fichier_out);
int concatenation(char *s1, char *s2, char **out);
//

int main(int argc, char ** argv) {

  int nb_bytes_copiees_total = 0;
  int i_fichiers_traites = 0;


    //controle parametres de lancement
    if (argc < 3 || argc%2==0) {
      return 1;
    }
    // ouverture du répertoire destination
    if ((mkdir("./copies",S_IRWXU | S_IRWXG | S_IRWXO)!=0) && (errno!=EEXIST)) { //on ignore si le repertoire est déjà créé
      printf("%d\n",nb_bytes_copiees_total);
      return 1;
    }

    while (i_fichiers_traites<(argc-1)/2) {

      char *PATH = argv[2*(i_fichiers_traites+1)-1];  // argv[1]
      char *ARGS = argv[2*(i_fichiers_traites+1)];    // argv[2]

      int OFFSET, BYTES;
      if (separer_OFFSET_BYTES(ARGS, &OFFSET, &BYTES) != 0) {
        printf("%d\n",nb_bytes_copiees_total);
        return 1;
      }

      // ouverture du fichier source
      int fd_lecture;
      fd_lecture = open(PATH, O_RDONLY);
      if (fd_lecture == -1) {
        printf("%d\n",nb_bytes_copiees_total);
        return 1;
      }
      if (file_size(fd_lecture)<OFFSET) {
        printf("%d\n",nb_bytes_copiees_total);
        return 1;
      }

      // ouverture du fichier destination
      char *nom_fichier;
      isoler_nom_fichier(PATH,&nom_fichier);

      char *path_ecriture;
      if (concatenation("./copies",nom_fichier,&path_ecriture) != 0) {
        printf("%d\n",nb_bytes_copiees_total);
        return 1;
      }

      int fd_ecriture;
      fd_ecriture = open(path_ecriture, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd_ecriture == -1) {
        printf("%d\n",nb_bytes_copiees_total);
        return 1;
      }


      // copier les octets
      char char_lu;
      int NB_BYTES_COPIEES = 0;
      while ( (NB_BYTES_COPIEES+OFFSET<BYTES+OFFSET) && (lseek(fd_lecture, NB_BYTES_COPIEES+OFFSET, SEEK_SET) < file_size(fd_lecture)) ) {

        // read
        if (lseek(fd_lecture, NB_BYTES_COPIEES+OFFSET, SEEK_SET) == -1) {
          printf("%d\n",nb_bytes_copiees_total);
          return 1;
        }
        if (read(fd_lecture, &char_lu, 1) == -1) {
          printf("%d\n",nb_bytes_copiees_total);
          return 1;
        }

        // write
        if (lseek(fd_ecriture, NB_BYTES_COPIEES, SEEK_SET) == -1) {
          printf("%d\n",nb_bytes_copiees_total);
          return 1;
        }
        if (write(fd_ecriture, &char_lu, 1) == -1) {
          printf("%d\n",nb_bytes_copiees_total);
          return 1;
        }

        NB_BYTES_COPIEES++; // ¡¡FALTA COMPROBACIÓN DE FIN DE FICHERO!!
        nb_bytes_copiees_total++;
      }
      if (NB_BYTES_COPIEES>file_size(fd_lecture) || NB_BYTES_COPIEES!=file_size(fd_ecriture)) {
        printf("%d\n",nb_bytes_copiees_total);
        return 1;
      }

      // clôture des fichiers
      if (close(fd_lecture) == -1) {
        printf("%d\n",nb_bytes_copiees_total);
        return 1;
      }
      if (close(fd_ecriture) == -1) {
        printf("%d\n",nb_bytes_copiees_total);
        return 1;
      }
      i_fichiers_traites++;
    }

    printf("%d\n",nb_bytes_copiees_total);
    return 0;
}

/* fonction : separer_OFFSET_BYTES(char *param, int *OFFSET,int *BYTES)
 * *param est la chaine de caractères qui correspond à argv[2], c'est-à-dire, "<OFFSET>-<BYTES>"
 * *OFFSET est un pointeur vers un entier où l'on va garder la valeur du offset
 * *BYTES est un pointeur vers un entier où l'on va garder la quantité des bytes à copier
 * But : separer la chaine en deux entiers
 * Sortie : 0 si succès, 1 si erreur
 * Note : le cas où on n'ha pas d'OFFSET (ex. "-5") va être traitré com si le offset soit 0 (ex. "-5" = "0-5")
 *        Pour désactiver cette 'feature', il faudrait enlever les barres de commentaire de l'estructure 'if' en commentaire
 */
int separer_OFFSET_BYTES(char *param, int *OFFSET,int *BYTES) {
    *OFFSET = 0;
    *BYTES = 0;
    int i = 0;
    // if (param[i]<'0' || '9'<param[i]) {return 1;} //verifier que c'est une chiffre
    while (param[i] != '-' && param[i] != '\0') { //convertir offset en entier et le garder dans le pointeur *OFFSET
      if (param[i]<'0' || '9'<param[i]) {return 1;} //verifier que c'est une chiffre
      *OFFSET = *OFFSET * 10;
      *OFFSET += param[i] - '0';
      i++;
    }
    if (param[i]!='-') {
      return 1;
    }
    else {
      i++;
      if (param[i] == '\0') {
        return 1;
      }
    }
    while (param[i] != '\0') {
      if (param[i]<'0' || '9'<param[i]) {return 1;} //verifier que c'est une chiffre
      *BYTES = *BYTES * 10;
      *BYTES += param[i] - '0';
      i++;
    }

    //Vous pouvez assumer que la taille maximale qui sera
    //copiée à partir d'un fichier ne dépassera pas 4096 octets.
    if (*BYTES > 4096) {
      *BYTES = 4096;
    }

    return 0;
}

/* fonction : ile_size(int fd)
 * fd est le "file descriptor" du fichier
 * Sortie : le nombre d'octets du fichier
 */
int file_size(int fd) {
    struct stat infos;
    fstat(fd, &infos);
    return infos.st_size;
}

/* fonction : isoler_nom_fichier(char *path, char **nom_fichier_out)
 * *path est la chaine de caractères qui correspond à argv[1], c'est-à-dire, le chemin au fichier source
 * *nom_fichier_out est un pointeur qui va pointer sur le premier élément de la chaine générée dans cette fonction
 * But : obtenir le nom du fichier à partir du path
 */
 void isoler_nom_fichier(char *path, char **nom_fichier_out) {

   char *checker = strrchr(path,'/');

   if (checker == NULL) { // fichier
     *nom_fichier_out = path;
     exit(2);
   }
   else { // fichier dans sous-répertoire
     *nom_fichier_out = checker;
   }
 }

/* fonction : concatenation(char *s1, char *s2, char *out)
 * *s1 est le premier String
 * *s1 est le deuxième String
 * *out est le pointeur qui pointe le String résultant
 * But : obtenir un String produit de la concaténation de s1 et s2
 * Sortie : 0 si succès, 1 si erreur
 */
int concatenation(char *s1, char *s2, char **out) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    if (len1<=0 || len2<=0) {
      return 1;
    }
    char r[len1+len2];
    *out = r;
    int i=0;
    while (i<len1) {
      r[i] = s1[i];
      i++;
    }
    i=0;
    while (i<len2) {
      r[i+len1] = s2[i];
      i++;
    }

    return 0;
}
