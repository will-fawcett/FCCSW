#ifndef LineParameters_h
#define LineParameters_h

// convert to new hit class 
#include "tricktrack/TTPoint.h"

// store parameters of a line 
class LineParameters {
  
  private:
    std::vector<std::pair<float, float> > m_coordinates;
    float m_gradient;
    float m_y_intercept;
    float m_x_intercept;

  public: 

    // default constructor
    LineParameters(){
      m_gradient = 0;
      m_y_intercept = 0;
      m_x_intercept =0;
    }

    // constructor
    LineParameters(std::vector<std::pair<float, float> > coordinates){
      m_coordinates = coordinates; 
    }

    // constructor for hit pointer
    LineParameters(std::vector<tricktrack::TTPoint*> hits){
      for(tricktrack::TTPoint* hit : hits){
        m_coordinates.push_back( std::make_pair(hit->z(), hit->rho() ) ); // coordinates in (z, r)
        // m_coordinates.emplace_back( hit->z(), hit->rho() ); // coordinates in (z, r) 
      }
    }

    // constructor for hit 
    LineParameters(std::vector<tricktrack::TTPoint> hits){
      for(const tricktrack::TTPoint hit : hits){
        m_coordinates.push_back( std::make_pair(hit.z(), hit.rho()) ); // coordinates in (z, r)
      }
    }

    // functions to calculate line parameters
    void simpleLinearLeastSquaresFit();
    void calculateLineParameters(float x0, float y0, float x1, float y1);

    // accessor functions
    float gradient() const{ return m_gradient;}
    float y_intercept() const { return m_y_intercept;}
    float x_intercept() const { return m_x_intercept;}

};

#endif // LineParameters_h
