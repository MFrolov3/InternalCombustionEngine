#include <iostream>
//#define INFO
#include <array>
using namespace std;


const double timeRate = 0.1;


class Engine
{
protected:
    string engineType; //тип двигателя
    bool isOperating; //работает или нет
    double tEngine; //темпратура двигателя, С°
    double tEnviroment; //температура окружающей среды, С°
    double tOverheating; //температура перегрева, С°
    double timeOperating; //время работы двигателя, с
public:
    void virtual RunEngine(double timeOperating) = 0; 
};

class InternalCombustionEngine : public Engine
{
private:
    double M; //Крутящий момент Н*м
    double V; //Скорость вращения рад/c
    double Hm; //Коэффициент зависимости скорости нагрева от крутящего момента
    double Hv; //Коэффициент зависимости скорости нагрева от скорости вращения коленвала
    double C; //Коэффициент зависимости скорости охлаждения от температуры двигателя и окружающей среды
    double I; //Момент инерции двигателя I (кг∙м2)
    double a; //Ускорение коленвала
    double Vh; //Скорость нагрева двигателя (С°/сек)
    double Vc; //Скорость охлаждения двигателя (С°/сек)
    array<double, 6> MdataPoints;
    array<double, 6> VdataPoints;


    void CalculateSpeadOfHeating()
    {
        Vh = M * Hm + pow(V, 2) * Hv;
    }

    void CalculateSpeadOfCooling() 
    {
        Vc = C * (tEnviroment - tEngine);
    }


public:
    //конструктор класса двигателя внутреннего сгорания
    InternalCombustionEngine
    (
        double tEnviroment = 25,
        double I = 10,
        double tOverheating = 110,
        double Hm = 0.01,
        double Hv = 0.0001,
        double C = 0.1,
        array<double, 6> MdataPoints = { 20, 75, 100, 105, 75, 0 },
        array<double, 6> VdataPoints = { 0, 75, 150, 200, 250, 300 }
    )
    {
        this->tEnviroment = tEnviroment;
        this->tEngine = tEnviroment;
        this->engineType = "InternalCombustion";
        this->I = I;
        this->tOverheating = tOverheating;
        this->Hm = Hm;
        this->Hv = Hv;
        this->C = C;
        this->MdataPoints = MdataPoints;
        this->VdataPoints = VdataPoints;
        this->M = 20;
        this->V = 0;
        this->timeOperating = 0;
        this->a = M / I;
    }

    //метод для запуска двигателя на определенное количество секунд
    void RunEngine(double timeRunning) 
    {
        for (int i = 0; i < timeRunning * 10; ++i)
        {
            V += a * timeRate;
            for (int j = 1; j < VdataPoints.size(); ++j)
            {
                if (V > VdataPoints[j-1] && V <= VdataPoints[j])
                {
                    M = (MdataPoints[j] - MdataPoints[j - 1]) / (VdataPoints[j] - VdataPoints[j - 1]) * (V - VdataPoints[j]) + MdataPoints[j];
                }
            }
            if (V >= VdataPoints[VdataPoints.size() - 1]) {
                M = 0;
            }
           
            a = M / I;
            CalculateSpeadOfHeating();
            CalculateSpeadOfCooling();
            tEngine += (Vh + Vc) * timeRate;
        }
        timeOperating += timeRunning;

    }

    //геттеры для всех полей класса
    string GetengineType() { return engineType; } 
    bool GetisOperating() { return isOperating; }
    double GettEngine() { return tEngine; }
    double GettEnviroment() { return tEnviroment; }
    double GettOverheating() { return tOverheating; }
    double GettimeOperating() { return timeOperating; }
    double GetM() { return M; }
    double GetV() { return V; }
    double GetHm() { return Hm; }
    double GetHv() { return Hv; } 
    double GetC() { return C; }
    double GetI() { return I; }
    double Geta() { return a; }
    double GetVh() { return Vh; }
    double GetVc() { return Vc; }

    //сеттеры при необходимости задать новые параметры двигателю
    //т.е. моделируем ситуацию когда двигатель прокачали))
    //а также задание температуры окружающей среды для моделирования
    //перегрева того же двигателя в других условиях
    void SetHm(double Hm) { this->Hm = Hm; }
    void SetHv(double Hv) { this->Hv = Hv; }
    void SetC(double C) { this->C = C; }
    void SetI(double I) { this->I = I; }
    void SetVh(double Vh) { this->Vh = Vh; }
    void SetVc(double Vc) { this->Vc = Vc; }
    void SettEnviroment(const double tEnviroment)
    {
        this->tEnviroment = tEnviroment; 
    }
    void SettEngine(const double tEngine)
    {
        this->tEngine = tEngine;
    }
    void SettOverheating(const double tOverheating)
    {
        this->tOverheating = tOverheating;
    }
};

double overheatTest(InternalCombustionEngine engine)
{
    // цикл работает до тех пор, пока температура двигателя <= температуры перегрева
    while (engine.GettEngine() <= engine.GettOverheating() - 0.001)
    {
        if (engine.GettEngine() > engine.GettOverheating() - 5)  // если температура двигателя близка к перегреву
            engine.RunEngine(0.1);                                // то запускаем двигатель на 0.1 секунды
        else                                                      // иначе
            engine.RunEngine(1);                                  // запускаем двигатель на 1 секунду
#ifdef INFO
        cout << engine.Get_engineRunTime() << "\t" << engine.Get_T_Engine() << "C°" << endl;
#endif
        if (engine.GettimeOperating() >= 3600)
        {
            cout << "The engine hasn't reached critical temperature" << endl;
            cout << "Max temperature while operating: " << engine.GettEngine() << "C°" << endl;
            exit(0);
        }
    }
    return engine.GettimeOperating();
}

int main()
{
    double tEnvironment;
    cout << "Enter temperature of enviroment in degrees of C";
    cin >> tEnvironment;
    InternalCombustionEngine engine = InternalCombustionEngine();
    engine.SettEngine(tEnvironment);
    engine.SettEnviroment(tEnvironment);
    cout << "Overheating time is: " << overheatTest(engine) << endl;
}
