import requests

class Assignment:
    def __init__(self, repo, branch, name, sha, timestamp):
        """
        Initialize the Assignment object with a name and base URL.

        :param name: The name of the assignment.
        :param base_url: The base URL of the assignment.
        """
        self.repo = repo
        self.branch = branch
        self.name = name
        self.sha = sha
        self.timestamp = timestamp
        
        
        


    def __str__(self):
        return f"{self.base_url}/assignments/{self.name}"
    
    def add_file_paths(self, branch_url):
        """
        Add file paths to the assignment from a specific branch.

        :param branch: The branch object from which to fetch file paths.
        """
