#include<ctime>
//生成0~1的随机数
static void Random_0_1(double rgb[])
{
	srand((unsigned int)time(NULL));
	rgb[0] = rand()*1.0 / RAND_MAX;
	rgb[1] = rand()*1.0 / RAND_MAX;
	rgb[2] = rand()*1.0 / RAND_MAX;
}

//bfs非递归版本
void Mesh::TraverseComponents(HEdge* he)
{
	if (he == NULL || he->Fl
		ag())
		return;
	double rgb[3];
	Random_0_1(rgb);
	Eigen::Vector3d color(rgb);

	std::queue<HEdge*> qIter;
	qIter.push(he);
	while (!qIter.empty())
	{
		HEdge* tmp = qIter.front();
		tmp->SetFlag(true);
		tmp->Start()->SetColor(color);
		tmp->End()->SetColor(color);
		qIter.pop();
		if (tmp->Next() != NULL && !(tmp->Next()->Flag()))
			qIter.push(tmp->Next());
		if (tmp->Prev() != NULL && !(tmp->Prev()->Flag()))
			qIter.push(tmp->Prev());
		if (tmp->Twin() != NULL && !(tmp->Twin()->Flag()))
			qIter.push(tmp->Twin());
	}
}

int Mesh::CountConnectedComponents() 
{
    /*************************/
    /* insert your code here */
	int count = 0;	
	HEdgeList elist = this->heList;
	for (size_t i = 0; i < elist.size(); ++i)
	{
		if (!elist[i]->Flag())
		{
			TraverseComponents(elist[i]);
			++count;
		}		
	}
    /*************************/
	std::cout << "CountOfComponents: " << count << std::endl;
	return count;
}