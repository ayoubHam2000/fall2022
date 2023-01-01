#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <sys/time.h>
#include <ctime>
#include <queue>
#include <stack>
#include <cmath>

#define LEFT (1)
#define RIGHT (-1)
#define PRINT_ERR(x) cerr << x << endl

using namespace std;

template <class Tp>
ostream& operator<<(ostream& os, const vector<Tp>& v){
	os << '[';
	for (auto item = v.begin(); item != v.end(); ++item){
		os << *item;
		if (item != v.end() - 1)
			os << ", ";
	}
	os << ']';
	return (os);
}




struct ARobot;

time_t get_time(void)
{
	struct timeval time_now;

	gettimeofday(&time_now, nullptr);
	time_t msecs_time = (time_now.tv_sec * 1e3) + (time_now.tv_usec / 1000);
	return (msecs_time);
}


struct APos{
public:
	int x;
	int y;
public:
	explicit APos(int x = -1, int y = -1): x(x), y(y){}
	APos(const APos& other) = default;
	void clear(){
		x = -1;
		y = -1;
	}

	APos& setPosFromIndex(int index, int width){
		x = index % width;
		y = index / width;
		return (*this);
	}

};

double distance(const double a, const double b){
	return std::sqrt((a)*(a) + (b)*(b));
}

double distance(const APos& a, const APos& b){
	return std::sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

bool operator==(const APos &a, const APos &b){
	return (a.x == b.x && a.y == b.y);
}

bool operator<(const APos &x, const APos &y){
	return (x.x < y.x || (y.x >= x.x && x.y < y.y));
}

ostream& operator<<(ostream& os, const APos& pos){
	os << "(" << pos.x << ", " << pos.y << ")";
	return (os);
}

struct APosInfo : public APos{
	int score;
	explicit APosInfo(int x = 0, int y = 0, int score = 0): score(score), APos(x, y){}
};

struct AMove{
public:
	int amount;
	int x;
	int y;
	int toX;
	int toY;
public:
	AMove(int amount, int x, int y, int toX, int toY):amount(amount), x(x), y(y), toX(toX), toY(toY) {}
	AMove(const AMove& other) = default;
	void clear(){
		amount = -1;
		x = -1;
		y = -1;
		toX = -1;
		toY = -1;
	}
};

bool operator==(const AMove &a, const AMove &b){
	return (a.x == b.x && a.y == b.y && a.toX == b.toX && a.toY == b.toY);
}

ostream& operator<<(ostream& os, const AMove& pos){
	os << "(" << pos.amount << " " << pos.x << " " << pos.y << pos.toX << " " << pos.toY << ")";
	return (os);
}

struct ASpawn{
public:
	int amount;
	int x;
	int y;
public:
	ASpawn(int amount, int x, int y) : amount(amount), x(x), y(y) {}
	ASpawn(const ASpawn& other) = default;
	void clear(){
		amount = -1;
		x = -1;
		y = -1;
	}
};

bool operator==(const ASpawn &a, const ASpawn &b){
	return (a.x == b.x && a.y == b.y);
}

ostream& operator<<(ostream& os, const ASpawn& pos){
	os << "(" << pos.amount << ", " << pos.x << ", " << pos.y << ")";
	return (os);
}

struct ARobot{
	int x;
	int y;
	int toX = -1;
	int toY = -1;

	explicit ARobot(int x = 0, int y = 0): x(x), y(y) {}

	void goTo(int a, int b){
		toX = a;
		toY = b;
	}

	void goTo(APos pos){
		toX = pos.x;
		toY = pos.y;
	}

};

struct Cell{
	int x{};
	int y{};
	int scrap_amount{};
	int owner{}; // 1 = me, 0 = foe, -1 = neutral
	int units{};
	int recycler{}; // 1 if there is a recycler on this cell.
	int can_build{};
	int can_spawn{};
	int in_range_of_recycler{};
	int	toX{-1};
	int toY{-1};
	int amount{0};


	APos getPos(){
		return APos(x, y);
	}

	void transformTo(Cell& cell, ARobot& robot){
		if (amount == 0)
			return ;
		if (cell.toX == toX && cell.toY == toY){
			amount = 0;
			toX = -1;
			toY = -1;
			return ;
		}
		cell.toX = toX;
		cell.toY = toY;
		robot.toX = toX;
		robot.toY = toY;
		cell.amount++;
		amount--;
		if (amount == 0){
			toX = -1;
			toY = -1;
		}
	}

};

struct Graph
{
public:
	std::vector<std::vector<int>>	adj;
	int								size{};

	explicit Graph(int n = 0)
	{
		adj.resize(n);
	}

	void addEdge(int u, int v)
	{
		adj[u].push_back(v);
		size++;
	}

	void clear(){
		adj.clear();
		size = 0;
	}

	bool depthFirstSearch(int startVertices, int endVertices){
		vector<bool> visited(size);
		_depthFirstSearch(startVertices, endVertices, visited);
		return visited[endVertices];
	}

	template <class Predictable>
	bool depthFirstSearch(int startVertices, Predictable f){
		vector<bool> visited(size);
		return _depthFirstSearch(startVertices, 24, visited, f);
	}

	template <class Predictable>
	bool	breathFirstSearch(int startVertices, Predictable f, int max_depth = 100){
		queue<int, list<int> > 		queue;
		vector<bool> 	visited(size);
		int 			current = -1;

		queue.push(startVertices);
		while (!queue.empty()){
			max_depth--;
			if (!max_depth)
				return (false);
			current = queue.front();
			queue.pop();
			if (f(current))
				return (true);
			visited[current] = true;
			for (int v : adj[current]) {
				if (!visited[v]){
					queue.push(v);
				}
			}
		}
		return (false);
	}

	bool	breathFirstSearch(int startVertices, int endVertices, vector<int>& path){
		queue<int> 		queue;
		vector<bool> 	visited(size);
		vector<int>		previous(size);
		int 			current = -1;

		queue.push(startVertices);
		while (!queue.empty()){
			current = queue.front();
			queue.pop();
			if (current == endVertices)
				break ;
			visited[current] = true;
			for (int v : adj[current]) {
				if (!visited[v]){
					previous[v] = current;
					queue.push(v);
				}
			}
		}
		if (current != endVertices)
			return (false);

		while (current != startVertices){
			path.push_back(current);
			current = previous[current];
		}
		path.push_back(current);
		std::reverse(path.begin(), path.end());
		return (true);
	}

	void print(int width = -1, ostream& os = cout){
		int i = 0;
		for (auto& item : adj){
			int x = i % width;
			int y = i / width;
			if (width != -1)
				os << i << ": " << APos(x, y) << " =>" << item << endl;
			else
				os << i << " =>" << item << endl;
			i++;
		}
	}

private:
	template <class Predictable>
	bool _depthFirstSearch(int s, int depth, std::vector<bool> &visited, Predictable f)
	{
		visited[s] = true;
		if (depth == 0)
			return (false);
		if (f(s))
			return (true);
		for (int v : adj[s]){
			if (!visited[v]){
				if (_depthFirstSearch(v, depth - 1, visited, f))
					return (true);
			}
		}
		return (false);
	}

	void _depthFirstSearch(int s, int e, std::vector<bool> &visited)
	{
		visited[s] = true;
		if (s == e)
			return ;
		for (int v : adj[s]){
			if (!visited[v]){
				_depthFirstSearch(v, e, visited);
			}
		}
	}
};

struct OutPut{
public:
	vector<APos >		buildPos;
	vector<ASpawn > 	spawnPos;
	vector<AMove > 		moves;
	std::string 		message;
	int&				matter;
public:
	explicit OutPut(int &matter) : matter(matter) {}

	void print(ostream& out = cout){
		int nb;

		nb = int(buildPos.size()) + int(spawnPos.size()) + int(moves.size()) + int(!message.empty());
		cerr << nb << endl;

		if (nb == 0){
			out << "WAIT" << endl;
		}

		if (nb == 1 && !message.empty()){
			out << "MESSAGE " << message << ";";
			out << "WAIT" << endl;
			return ;
		}

		for (auto& item : buildPos){
			out << "BUILD " << item.x << " " << item.y;
			(nb > 1) ? (out << ";") : (out << endl); nb--;
		}

		for (auto& item : moves){
			out << "MOVE " << item.amount << " " << item.x << " " << item.y << " " << item.toX << " " << item.toY;
			(nb > 1) ? (out << ";") : (out << endl); nb--;
		}

		for (auto& item : spawnPos){
			out << "SPAWN " << item.amount << " " << item.x << " " << item.y;
			(nb > 1) ? (out << ";") : (out << endl); nb--;
		}

		if (!message.empty()){
			out << "MESSAGE " << message;
			(nb > 1) ? (out << ";") : (out << endl); nb--;
		}

		reset();
	}

	bool insertToBuildPos(const APos& pos){
		auto isFound = std::find(buildPos.begin(), buildPos.end(), pos);
		if (isFound == buildPos.end()){
			PRINT_ERR("matter : " << matter << "build in => " << pos << " const: 10");
			matter -= 10;
			buildPos.emplace_back(pos);
			return (true);
		}
		return (false);
	}

	bool insertToSpawn(const ASpawn& pos){
		auto isFound = std::find(spawnPos.begin(), spawnPos.end(), pos);
		if (isFound == spawnPos.end()){
			PRINT_ERR("matter : " << matter << "spawn => " << pos << " const: " << 10 * pos.amount);
			matter -= 10 * pos.amount;
			spawnPos.emplace_back(pos);
			return (true);
		}
		return (false);
	}

	bool insertToMoves(const AMove& pos){
		auto isFound = std::find(moves.begin(), moves.end(), pos);
		if (isFound == moves.end()){
			moves.emplace_back(pos);
			return (true);
		}
		return (false);
	}

	void reset(){
		buildPos.clear();
		spawnPos.clear();
		moves.clear();
		message.clear();
	}
};

struct Result{
	vector<APosInfo> 	recyclerCellsScore;
	vector<ARobot>		myRobots;
	vector<ARobot>		oppRobots;
	vector<APos>		myRecyclers;
	vector<APos>		oppRecyclers;
	vector<APos>		myCells;
	vector<APos>		activeCells;
	vector<APos>		oppCells;
	vector<APos>		neutralCells;
	Graph				graph;
	int					center{};

	int myPosition{}; //once
	int	nbMyCells{};
	int nbOppCells{};
	int nbMyRobots{};
	int nbOppRobots{};

	void clear(){
		recyclerCellsScore.clear();
		myRobots.clear();
		oppRobots.clear();
		myRecyclers.clear();
		oppRecyclers.clear();
		myCells.clear();
		oppCells.clear();
		neutralCells.clear();
		//graph.clear();

		nbMyCells = 0;
		nbOppCells = 0;
		nbMyRobots = 0;
		nbOppRobots = 0;
		center = 0;
	}
};

struct Game{
	int width;
	int height;
	int my_matter;
	int opp_matter;
	int frame;
	time_t time;
	vector<Cell> cells;
	OutPut	out;
	Result	result;
public:
	Game(int w, int h):
			width(w),
			height(h),
			my_matter(),
			opp_matter(),
			frame(),
			time(get_time()),
			cells(vector<Cell>(height * width)),
			out(my_matter)
	{

	}
public:
	void readInput(istream& in = cin){
		in >> my_matter >> opp_matter; in.ignore();
		time = get_time();
		//cells.clear();
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				Cell& cell = cells[y * width + x];
				cell.x = x;
				cell.y = y;
				in >> cell.scrap_amount >> cell.owner >> cell.units >> cell.recycler >> cell.can_build >> cell.can_spawn >> cell.in_range_of_recycler; in.ignore();

				if (cell.scrap_amount == 0 || cell.recycler || cell.owner != 1){
					cell.toX = -1;
					cell.toY = -1;
					cell.amount = 0;
				}
			}
		}
	}

	void printTime() const{
		cerr << get_time() - time << "ms" << endl;
	}

	Cell& getCell(int x, int y){
		return (cells[y * width + x]);
	}

	int getCellIndex(int x, int y){
		return (x + y * width);
	}

	int getCellIndex(APos pos){
		return (pos.x + pos.y * width);
	}

	Cell& getCell(const APos& pos){
		return (cells[pos.y * width + pos.x]);
	}

	vector<Cell*>	getAdjacentCells(const APos& pos){
		vector<Cell*>	res(4);


		res[0] = pos.x - 1 >= 0 ? &getCell(pos.x - 1, pos.y) : nullptr;
		res[1] = pos.x + 1 < width ? &getCell(pos.x + 1, pos.y) : nullptr;
		res[2] = pos.y - 1 >= 0 ? &getCell(pos.x, pos.y - 1) : nullptr;
		res[3] = pos.y + 1 < height ? &getCell(pos.x, pos.y + 1) : nullptr;

		return (res);
	}

	int getCellRecyclerProductivity(const APos& pos){
		int res = 0;
		vector<Cell*>	adjacentCells = getAdjacentCells(pos);

		adjacentCells.push_back(&getCell(pos));
		for (auto& i : adjacentCells){
			if (i && i->owner)
				res += i->scrap_amount;
		}
		return (res);
	}

	void getRecyclerCellsScore(){
		vector<APosInfo>& recyclerCellsScore = result.recyclerCellsScore;

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				Cell& cell = getCell(x, y);
				if (cell.can_build){
					APosInfo	pos(cell.x, cell.y);
					pos.score = getCellRecyclerProductivity(pos);
					recyclerCellsScore.push_back(pos);
				}
				std::sort(recyclerCellsScore.begin(), recyclerCellsScore.end(), [](APosInfo &a, APosInfo &b) -> bool {
					return a.score < b.score;
				});
			}
		}
	}

	static bool isBlock(const Cell& cell){
		return (cell.scrap_amount == 0 || (cell.scrap_amount == 1 && cell.in_range_of_recycler) || cell.recycler);
	}

	void setGraph(){
		result.graph = Graph(width * height);
		int arr[][2] = {{-1, 0}, {+1, 0}, {0, -1}, {0, 1}};

		PRINT_ERR(result.graph.adj.size());
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if (getCell(x, y).scrap_amount == 0)
					continue;
				for (auto& k : arr){
					int a = x + k[0];
					int b = y + k[1];
					if (a >= 0 && a < width && b >= 0 && b < height && !isBlock(getCell(a, b))){
						result.graph.addEdge(x + (y * width), a + (b * width));
					}
				}
			}
		}
	}

	int countHalf(int s, int e){
		int res = 0;
		for (int y = 0; y < height; y++){
			for (int x = s; x < e; x++){
				if (getCell(x, y).owner != 1 && getCell(x, y).scrap_amount){
					res++;
				}
			}
		}
		return res;
	}

	int getCenter(){
		int s = 0;
		int e = width;
		int depth = 3;
		const int diff = height * 3;
		int center = (e - s) / 2;
		while (depth--){
			int leftCount = countHalf(0, center);
			int rightCount = countHalf(center, width);
			if (abs(leftCount - rightCount) < diff) {
				break;
			}
			if (leftCount > rightCount){
				e = width / 2;
			}else{
				s = width / 2;
			}
			center = (e - s) / 2;
		}
		return center;
	}

	void getInfo(){
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				Cell& cell = getCell(x, y);
				//cerr << cell.x << " " << cell.y << " | ";
				//cerr << APos(cell.targetCell) << " " << APos(cell.targetBy) << " | ";
				if (cell.owner == 1){
					result.nbMyCells++;
					result.nbMyRobots += cell.units;
					result.myCells.emplace_back(x, y);
					if (cell.toX != -1 && cell.toY != -1)
						result.activeCells.emplace_back(x, y);
					int tmp = cell.units;
					while (tmp--)
						result.myRobots.emplace_back(x, y);
					if (cell.recycler) {
						result.myRecyclers.emplace_back(x, y);
					}
				}
				else if(cell.owner == 0){
					result.nbOppCells++;
					result.nbOppRobots += cell.units;
					result.oppCells.emplace_back(x, y);
					int tmp = cell.units;
					while (tmp--)
						result.oppRobots.emplace_back(x, y);
					if (cell.recycler) {
						result.myRecyclers.emplace_back(x, y);
					}
				} else {
					result.neutralCells.emplace_back(x, y);
				}
			}

			//cerr << endl;
		}
		result.center = getCenter() / 2;
		setGraph();
		//result.graph.print(width, cerr);
		if (result.myPosition == 0){
			ARobot& a = result.myRobots[0];
			ARobot& b = result.oppRobots[0];
			result.myPosition = a.x < b.x ? LEFT : RIGHT;
		}
		PRINT_ERR("Frame= " << frame );
		PRINT_ERR("Matter= " << my_matter);
		PRINT_ERR("w= " << width );
		PRINT_ERR("h= " << height );
		PRINT_ERR("result.myRobots= " << result.myRobots.size() );
		PRINT_ERR("result.myRobots Units= " << result.nbMyRobots );
		PRINT_ERR("result.oppRobots= " << result.oppRobots.size() );
		PRINT_ERR("result.nbMyCells= " << result.nbMyCells );
		PRINT_ERR("result.nbOppCells= " << result.nbOppCells );
		PRINT_ERR("result.myPosition= " << (result.myPosition == LEFT ? "Left" : "Right") );
		PRINT_ERR("center: " << result.center);
	}



	void getRobotNextMove(){
		vector<pair<Cell*, double> > cellsScore;
		vector<APos> leftCells;
		cellsScore.reserve(width * height);
		leftCells.reserve(width * height / 2);
		APos center(result.center * 2, height / 2);
		double maxDist = distance(width / 2.0, height / 2.0);
		int i = 0;
		PRINT_ERR("start Cells");
		for (Cell& cell : cells){
			double score = -1;
			if (cell.recycler || cell.scrap_amount == 0){
				score = 0;
			}
			if (score == -1 && cell.scrap_amount == 1){
				vector<Cell*> adjacentCells = getAdjacentCells(APos(cell.x, cell.y));
				for (auto adj : adjacentCells){
					if (adj && adj->recycler){
						score = 0;
						break ;
					}
				}
			}
			if (score == -1){
				APos cellPos(cell.x, cell.y);
				score = distance(cellPos, center);
				score = score < 1 ? maxDist : ((1.0 / score) * maxDist);
				if (cell.owner == 0 && cell.units < 1)
					score += maxDist * 0.8;
				else if (cell.owner == 0 && cell.units >= 1)
					score += maxDist * (0.2 / cell.units);
				if (cell.owner == 1 && cell.units < 1)
					score -= maxDist;
				else if (cell.owner == 1 && cell.units >= 1)
					score -= maxDist * (0.5 * cell.units);
				if (cell.owner == -1)
					score += maxDist * 0.3;
			}
			if (score == -1)
				score = 0;
			cellsScore.emplace_back(&cell, score);

			/*cerr.precision(3);
			cerr << score << ", ";
			i++;
			if (i == width){
				cerr << endl;
				i = 0;
			}*/

			//left
			//if (cell.owner == -1 && cell.scrap_amount && ((cell.x > center.x && result.myPosition == RIGHT) || (cell.x < center.x && result.myPosition == LEFT))){
			//	leftCells.emplace_back(cell.x, cell.y);
			//}

		}

		std::remove_if(cellsScore.begin(), cellsScore.end(), [this](const pair<Cell*, double>& a) -> bool {
			return (a.second == 0 || !this->result.graph.depthFirstSearch(this->getCellIndex(a.first->x, a.first->y), [this](int v) -> bool {
				return this->cells[v].units > 1 && this->cells[v].owner == 1;
			}));
		});

		std::sort(cellsScore.begin(), cellsScore.end(), [](const pair<Cell*, double>& a, const pair<Cell*, double>&b)->bool{
			return b.second < a.second;
		});
		PRINT_ERR("end Cells");

		//Robot
		PRINT_ERR("start ARobot");
		std::sort(result.myRobots.begin(), result.myRobots.end(), [center](const ARobot& a, const ARobot& b){
			return (distance(APos(a.x, a.y), center) < distance((APos(b.x, b.y)), center));
		});


		int cellIndex = 0;
		int size = (int)cellsScore.size();
		int leftSize = (int)leftCells.size();
		int robotSize = int(result.myRobots.size());
		int robotIndex = robotSize;
		for (ARobot& robot : result.myRobots){
			//PRINT_ERR("From " << APos(robot.x, robot.y));
			robotIndex--;
			if (cellIndex < 2){
				auto res = std::find(result.activeCells.begin(), result.activeCells.end(), APos(robot.x, robot.y));
				if (res != result.activeCells.end()){
					Cell& cell = getCell(robot.x, robot.y);
					Cell& toCell = getCell(cell.toX, cell.toY);
					cell.transformTo(toCell, robot);
				}
				if (robot.toX == -1){
					int closest = getCellIndex(center.x, center.y);
					result.graph.breathFirstSearch(getCellIndex(robot.x, robot.y), [this, &closest](int v){
						if (this->cells[v].owner != 1 && this->cells[v].scrap_amount && this->cells[v].x < this->width / 2 - 2){
							closest = v;
							return (true);
						}
						return (false);
					});
					APos pos = APos().setPosFromIndex(closest, width);
					Cell& cell = getCell(pos.x, pos.y);
					cell.amount++;
					cell.toX = pos.x;
					cell.toY= pos.y;
					robot.goTo(pos);
				}
				//PRINT_ERR("From " << APos(robot.x, robot.y) << " To " << pos);
			}
			else if (robotIndex > 2){
				Cell& cell = *cellsScore[cellIndex % size].first;
				robot.goTo(cell.getPos());
			}else{
				auto res = std::find(result.activeCells.begin(), result.activeCells.end(), APos(robot.x, robot.y));
				if (res != result.activeCells.end()){
					Cell& cell = getCell(robot.x, robot.y);
					Cell& toCell = getCell(cell.toX, cell.toY);
					cell.transformTo(toCell, robot);
				}
				if (robot.toX == -1){
					int closest = getCellIndex(center.x, center.y);
					result.graph.breathFirstSearch(getCellIndex(robot.x, robot.y), [this, &closest](int v){
						if (this->cells[v].owner != 1 && this->cells[v].scrap_amount && this->cells[v].x > this->width / 2 + 2){
							closest = v;
							return (true);
						}
						return (false);
					});
					APos pos = APos().setPosFromIndex(closest, width);
					Cell& cell = getCell(pos.x, pos.y);
					cell.amount++;
					cell.toX = pos.x;
					cell.toY= pos.y;
					robot.goTo(pos);
				}
				//PRINT_ERR("From " << APos(robot.x, robot.y) << " To " << pos);
			}
			cellIndex++;
		}
		PRINT_ERR("end ARobot");

		for (ARobot& robot : result.myRobots){
			if (robot.toX != -1)
				out.insertToMoves(AMove(1, robot.x, robot.y, robot.toX, robot.toY ));
		}
	}


	void spawnRobotOrBuild(){
		cerr << "MyMatter: " << my_matter << endl;
		std::set<APos, less<> > buildPos;
		std::set<APos, less<> > spawnPos;

		APos center(result.center, height / 2);
		std::sort(result.myCells.begin(), result.myCells.end(), [center](const APos& a, const APos& b){
			return (distance(b, center) >= distance(a, center));
		});

		int timeToDie = 3;
		if (my_matter > 100)
			timeToDie *= 3;

		int index = 0;
		while (my_matter >= 10 && result.nbMyCells && timeToDie){
			int flag = 0;
			for (auto& enemy : result.oppRobots){
				vector<Cell *> adjacentCells = getAdjacentCells(APos(enemy.x, enemy.y));
				for (auto item : adjacentCells){
					if (!item)
						continue;
					int add = result.myPosition == LEFT ? -1 : +1;
					if (item->owner == 1 && item->recycler == 0 && enemy.x + add == item->x){
						if (item->units == 0 &&
							((result.myPosition == LEFT && item->x < width / 2 - 2) || (result.myPosition == RIGHT && item->x > width / 2 + 2))){
							//out.insertToSpawn(ASpawn(1, item->x, item->y));

							out.insertToBuildPos(APos(item->x, item->y));
							flag = 1;


						}else if (item->units == 1){
							out.insertToSpawn(ASpawn(1, item->x, item->y));
							flag = 1;
						}
						if (flag == 1)
							break;
					}
				}
				if (flag == 1)
					break;
			}

			if (flag == 0 && !result.myCells.empty()){
				Cell& defaultCell = getCell(result.myCells[index % result.myCells.size()]);
				index++;
				/*auto isFound = std::find_if(result.myCells.begin(), result.myCells.end(), [this](APos pos) {
					Cell& cell = getCell(pos);
					return cell.units == 0;
				});*/
				//if (isFound == result.myCells.end())
				//	out.insertToSpawn(ASpawn(1, defaultCell.x, defaultCell.y));
				//else{
				//	Cell& cell = getCell(*isFound);
				out.insertToSpawn(ASpawn(1, defaultCell.x, defaultCell.y));
				//}
			}
			timeToDie--;
		}
	}

}; //End Game Class

void start(Game& game){
	game.getInfo();
	//=> scan
	//game.getRecyclerCellsScore();

	//=> use
	game.getRobotNextMove();
	game.spawnRobotOrBuild();

	game.result.clear();
	game.out.message = to_string((get_time() - game.time)) + "ms";
}

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

void testFun(){
	char p[][11] = {
			"1111111111",
			"1000000001",
			"1111111001",
			"1000001001",
			"1001001001",
			"1001000001",
			"1111111111"
	};
	int arr[][2] = {{-1, 0}, {+1, 0}, {0, -1}, {0, 1}};
	// Create a graph given in the above diagram
	int width = 10;
	int height = 7;
	Graph g(width * height);
	for (int y = 0; y < height; y++){
		for (int x = 0; x < width; x++){
			for (auto& k : arr){
				int a = x + k[0];
				int b = y + k[1];
				if (a >= 0 && a < width && b >= 0 && b < height && p[b][a] != '1'){
					g.addEdge(x + (y * width), a + (b * width));
				}
			}
		}
	}
	g.print(width);
	cout << g.depthFirstSearch(11, [p](int v)->bool {
		int x = v % 10;
		int y = v / 10;
		return (p[y][x] == '2');
	}) << endl;
	/*vector<int> path;
	g.breathFirstSearch(10, 69, path);
	for (int item : path){
		int x = item % width;
		int y = item / width;
		cout << APos(x, y) << " -> ";
	}
	cout << endl;*/



}

int main()
{
	game();
	//testFun();


	return 0;
}