/*******************************************/
/*     created by aben-ham 1/1/23         */
/*******************************************/

#include "code/headers/Cell.h"
#include "code/headers/Game.h"
#include "code/headers/Out.h"
#include "code/headers/Result.h"
#include "code/utils.h"

[[noreturn]] void game(){
	int width;
	int height;
	cin >> width >> height; cin.ignore();
	Game game(width, height);
	while (true) {
		game.readInput();
		game.frame++;
		game.out.message = to_string(game.frame);
		start(game);
		game.out.print();
	}
}

void test(){

}

int main()
{
	game();
	//test();
}
