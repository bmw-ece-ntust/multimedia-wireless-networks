import requests
from assignment import Assignment

class Branch:
    """
    A class representing a branch in a version control system.
    """

    def __init__(self, owner, repo, name):
        """
        Initialize the Branch object with a name and base URL.

        :param name: The name of the branch.
        :param base_url: The base URL of the repository.
        """
        self.owner = owner
        self.name = name
        self.repo = repo
        self.branch_url = f"https://api.github.com/repos/{owner}/{repo}/branches/{name}"
        self.branch_tree_url = f"https://api.github.com/repos/{owner}/{repo}/git/trees/{name}?recursive=1"
        self.assignments = []

    def __str__(self):
        return f"{self.branch_tree_url}"

    def get_repo(self):
        """
        Get the repository associated with the branch.

        :return: The repository object.
        """
        return self.repo

    def add_assignments(self, filter=""):
        """
        Add assignments to the branch.

        :param filter: A filter string to match assignment names.
        """
        url = f"{self.repo}/branches/{self.name}/assignments"
        try:
            response = requests.get(url)
            response.raise_for_status()
            assignments = response.json()

            assignment_names = [
                assignment["name"] for assignment in assignments if filter in assignment["name"]
            ]

            for assignment in assignment_names:
                self.assignments.append(assignment)
            return self.assignments
        except requests.exceptions.RequestException as e:
            print(f"Error fetching assignments: {e}")