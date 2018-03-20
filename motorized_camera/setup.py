from setuptools import setup

setup(
    name='motorized_camera',
    version='1.0',
    packages=['motorized_camera'],
    install_requires=['pyserial'],
    include_package_data=True,
    url='',
    license='GPL',
    author='gvtech',
    author_email='',
    description='Control a camera yaw and pitch from a web browser'
)
