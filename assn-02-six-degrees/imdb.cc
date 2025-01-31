using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "imdb.h"
#include <stdio.h>
#include <string.h>

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
	    (movieInfo.fd == -1) ); 
}

struct playerSt{
  const void * actorFile1;
  string player1;
};

int comparPlayer(const void *player1, const void *player2){
  int location = *((int*)player2);
  const char* player_2 = ((char *)(((playerSt *)player1)->actorFile1) + location);
  const char* player_1 = ((playerSt *)player1)->player1.c_str();
  return strcmp(player_1, player_2);
}

// you should be implementing these two methods right here... 
bool imdb::getCredits(const string& player, vector<film>& films) const { 
  playerSt player_St;
  player_St.actorFile1 = actorFile;
  player_St.player1 = player;
  int num = ((int *)actorFile)[0];
  void *playerBase = bsearch(&player_St, (int*)actorFile + 1, num, sizeof(int), comparPlayer);
  if(playerBase == nullptr) return false;
  int realPlayerStart = *((int*)playerBase);
  void * realPlayerBase = (char*)actorFile + realPlayerStart;
  int nameEnds = (player.length() + 1);
  if((player.length() + 1) % 2 != 0){
    nameEnds++;
  }
  short movieCount = *((short*)((char*)realPlayerBase + nameEnds));
  int movieStarts = nameEnds;
  if((nameEnds + 2) % 4 != 0){
    movieStarts += 4;
  } else {
    movieStarts += 2;
  }
  for(int i = 0; i < movieCount; i++){
    int movieLoc = *((int *)((char *)realPlayerBase + movieStarts));
    movieStarts += 4;
    film actorFilm;
    actorFilm.title = ((char *)movieFile + movieLoc);
    actorFilm.year = 1900 + *((char*)movieFile + movieLoc + actorFilm.title.length() + 1);
    films.push_back(actorFilm);
  }
  return true; 
}

struct movieSt{
  const void * movieFile1;
  film movie1;
};

int comparMovie(const void *movie1, const void *movie2){
  int movieLoc = *(int*)movie2;
  film film1 = ((movieSt*)movie1) -> movie1;
  film film2;
  film2.title = ((char *)(((movieSt*)movie1) -> movieFile1)) + movieLoc;
  film2.year = 1900 + *(((char *)(((movieSt*)movie1) -> movieFile1)) + movieLoc + film2.title.length() + 1);
  if(film1 == film2) return 0;
  if(film1 < film2) return -1;
  return 1;
}

bool imdb::getCast(const film& movie, vector<string>& players) const {
  movieSt movie_St;
  movie_St.movieFile1 = movieFile;
  movie_St.movie1 = movie;
  int num = ((int*)movieFile)[0];
  void *movieBase = bsearch(&movie_St, (int*)movieFile + 1, num, sizeof(int), comparMovie);
  if(movieBase == nullptr) return false;
  int movieStartLoc = *((int *)movieBase);
  int movieYearLoc = movieStartLoc + movie.title.length() + 1;
  int actorNumLoc = movieYearLoc + 1 - movieStartLoc;
  if(actorNumLoc % 2 != 0){
    actorNumLoc++;
  }
  actorNumLoc +=movieStartLoc;
  short actorNum = *((short*)((char*)movieFile + actorNumLoc));
  int actorLoc = actorNumLoc - movieStartLoc + 2;
  if(actorLoc % 4  != 0) actorLoc += 2;
  actorLoc += movieStartLoc;
  for(int i = 0; i < actorNum; i++){
    string newActor = (char*)actorFile + *((int *)((char*)movieFile + actorLoc));
    players.push_back(newActor);
    actorLoc += 4;
  }
  return true;
}

imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
