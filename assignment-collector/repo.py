import requests
from branch import Branch


class Repo:
    def __init__(self, owner, name):
        self.owner = owner
        self.name = name
        self.base_url = f"https://api.github.com/repos/{owner}/{name}"
        self.branches = []

    def __str__(self):
        return self.base_url

    def get_owner(self):
        return self.owner

    def get_name(self):
        return self.name

    def add_branch(self, filter=""):
        url = f"{self.base_url}/branches"
        try:
            response = requests.get(url)
            response.raise_for_status()
            branches = response.json()

            branch_names = [
                branch["name"] for branch in branches if filter in branch["name"]
            ]

            for branch in branch_names:
                branch = Branch(
                    self.owner, self.name, branch
                )  # Verify that the Branch class constructor accepts these arguments
                self.branches.append(branch)
            return self.branches
        except requests.exceptions.RequestException as e:
            print(f"Error fetching branches: {e}")
