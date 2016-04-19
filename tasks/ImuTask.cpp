/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "ImuTask.hpp"

using namespace std;
using namespace gazebo;
using namespace rock_gazebo;

ImuTask::ImuTask(std::string const& name)
    : ImuTaskBase(name)
{
}

ImuTask::ImuTask(std::string const& name, RTT::ExecutionEngine* engine)
    : ImuTaskBase(name, engine)
{
}

ImuTask::~ImuTask()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See ImuTask.hpp for more detailed
// documentation about them.

bool ImuTask::configureHook()
{
    if (! ImuTaskBase::configureHook())
        return false;

    // Initialize communication node and subscribe to gazebo topic
    node = transport::NodePtr( new transport::Node() );
    node->Init();

    imu_subscriber = node->Subscribe("~/" + topicName, &ImuTask::readInput, this);
    gzmsg << "ImuTask: subscribing to gazebo topic ~/" + topicName << endl;

    return true;
}
bool ImuTask::startHook()
{
    if (! ImuTaskBase::startHook())
        return false;
    return true;
}
void ImuTask::updateHook()
{
    ImuTaskBase::updateHook();
    if(imu_reading.hasValidOrientation())
    {
        imu_reading.sourceFrame = _imu_frame.value();
        imu_reading.targetFrame = _world_frame.value();
        _orientation_samples.write(imu_reading);
        imu_reading.invalidate();
    }
}
void ImuTask::errorHook()
{
    ImuTaskBase::errorHook();
}
void ImuTask::stopHook()
{
    ImuTaskBase::stopHook();
}
void ImuTask::cleanupHook()
{
    ImuTaskBase::cleanupHook();
    node->Fini();
}

void ImuTask::setGazeboModel( ModelPtr model, string sensorName, string topicName )
{
    string taskName = "gazebo:" + model->GetWorld()->GetName() + ":" + model->GetName() + ":" + sensorName;
    if(!provides())
        throw std::runtime_error("ImuTask::provides returned NULL");
    provides()->setName(taskName);
    _name.set(taskName);
    BaseTask::setGazeboWorld( model->GetWorld() );

    // Set topic name to communicate with Gazebo
    this->topicName = topicName;
}

void ImuTask::readInput( ConstIMUPtr & imuMsg)
{
    if(!imuMsg->has_orientation())
        throw std::runtime_error("rock_gazebo::ImuTask requires has_orientation");
    if(!imuMsg->has_angular_velocity())
        throw std::runtime_error("rock_gazebo::ImuTask requires has_angular_velocity");

    const ::gazebo::msgs::Quaternion &quat = imuMsg->orientation();
    if(!quat.has_x() || !quat.has_y() || !quat.has_z() || !quat.has_w())
        throw std::runtime_error("rock_gazebo::ImuTask: invalid quaternion");
    imu_reading.orientation = base::Orientation(quat.w(),quat.x(),quat.y(),quat.z());

    const ::gazebo::msgs::Vector3d& avel =  imuMsg->angular_velocity();
    if(!avel.has_x() || !avel.has_y() || !avel.has_z())
        throw std::runtime_error("rock_gazebo::ImuTask: invalid Vector3d");
    imu_reading.angular_velocity = base::Vector3d(avel.x(),avel.y(),avel.z());
    imu_reading.time = getCurrentTime();
}

