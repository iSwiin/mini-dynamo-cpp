pipeline {
  agent any
  stages {
    stage('Checkout') {
      steps { checkout scm }
    }
    stage('Configure') {
      steps {
        sh '''
          cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON -DENABLE_UBSAN=ON
        '''
      }
    }
    stage('Build') {
      steps {
        sh 'cmake --build build -j'
      }
    }
    stage('Test') {
      steps {
        sh 'ctest --test-dir build --output-on-failure'
      }
    }
  }
}
