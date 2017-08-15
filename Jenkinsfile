pipeline {
    agent {
        docker {
            image 'namedkitten/cpp-bot'
	          args  '-u 0'
        }
    }
    environment {
        WEBHOOK_URL = credentials('WEBHOOK_URL')
	      TOKEN = credentials('TOKEN')
        PREFIX = "nya++|"
    }
    stages {
        stage('Test') {
            steps {
                echo "Doing the build."
                sh 'bash build.sh'
		sh 'curl -T build/KittehBot++ chunk.io -s'
            }
        }
    }
    post {
        success {
        sh 'python3 jenkins.py success $(git --no-pager show -s --format=\'%an\' HEAD~) $(git log --format="%H" -n 1)'
        }
        failure {
        sh 'python3 jenkins.py failure $(git --no-pager show -s --format=\'%an\' HEAD~) $(git log --format="%H" -n 1)'
        }
        unstable {
        sh 'python3 jenkins.py unstable $(git --no-pager show -s --format=\'%an\' HEAD~) $(git log --format="%H" -n 1)'
        }
    }
}
