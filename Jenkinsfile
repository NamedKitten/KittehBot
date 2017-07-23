pipeline {
    agent {
        docker {
            image 'namedkitten/cpp-env'
	    args  '-u 0'
        }
    }
    environment { 
        WEBHOOK_URL = credentials('WEBHOOK_URL') 
    }
    stages {
        stage('Compile') {
            steps {
                sh 'build.sh'
            }
        }
    }
    post {
        always {
            sh 'pip3 install requests'
        }
        success {
        sh 'python3.5 jenkins.py success $(git --no-pager show -s --format=\'%an\' HEAD~) $(git log --format="%H" -n 1)'
        }
        failure {
        sh 'python3.5 jenkins.py failure $(git --no-pager show -s --format=\'%an\' HEAD~) $(git log --format="%H" -n 1)'
        }
        unstable {
        sh 'python3.5 jenkins.py unstable $(git --no-pager show -s --format=\'%an\' HEAD~) $(git log --format="%H" -n 1)'
        }
    }
}
